
// Define connections to the shift register
const int latchPin = 11;  // Connects to STCP (latch pin) on the shift register
const int clockPin = 10;  // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;   // Connects to DS (data pin) on the shift register

const int startButtonPin = 3;
const int resetButtonPin = 0;
const int lapButtonPin = 2;

// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
// Store the digits in an array for easy access
const int displayCount = 4;  // Number of digits in the display
int displayDigits[displayCount] = { segD1, segD2, segD3, segD4 };
// Define the number of unique encodings (0-9)
const int encodingsNumber = 10;
// Define byte encodings for the digits
byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
};

// Variables for controlling the display update timing
unsigned long lastIncrement = 0;
unsigned long delayCount = 100;  // Delay between updates (milliseconds)
unsigned long number = 0;        // The number being displayed

const byte decimalPointPosition = 2; // The display with the DP
bool counting = false; // Flag for counting mode
bool paused = false; // Flag for paused mode 
bool viewing = false; // Flag for viewing mode

const int lapMemorySize = 4; // Max number of laps to be saved
int lapMemory[] = { 0, -1, -1, -1, -1 }; // Vector holding the saved laps and a position from where we can start counting again
int lapSavingPosition = 1; // Index for saving laps in the memory
int lapCyclePosition = 0; // Index for cycling through laps in viewing mode

const unsigned long debounceDelay = 50;

volatile bool startPausePossiblePress = false; // Indicates that a button press might have occurred
volatile bool startPauseShouldStartCounter = false; // Indicates the need to start debounce counter
bool startPauseState = HIGH; // The previous state of the button (HIGH when not pressed due to INPUT_PULLUP)
bool startPauseReading = HIGH; // The current reading from the button pin
unsigned long startPauseLastDebounceTime = 0; // Keeps track of the last time the button was pressed

bool resetState = HIGH; // The previous state of the button (HIGH when not pressed due to INPUT_PULLUP)
bool resetReading = HIGH; // The current reading from the button pin
bool lastResetState = HIGH; 
unsigned long resetLastDebounceTime = 0; // Keeps track of the last time the button was pressed

volatile bool lapPossiblePress = false; // Indicates that a button press might have occurred
volatile bool lapPossibleLongPress = false; // Indicates that a button press might have occurred
volatile bool lapShouldStartCounter = false; // Indicates the need to start debounce counter
bool lapState = HIGH; // The previous state of the button (HIGH when not pressed due to INPUT_PULLUP)
bool lapReading = HIGH; // The current reading from the button pin
unsigned long lapLastDebounceTime = 0; // Keeps track of the last time the button was pressed
unsigned long lapCycleDelay = 2500; // Delay for when holding the lap button
volatile bool endLapCycle = false; // Flag used for ending the cycle after releasing a continous press


void setup() {
  // Initialize the pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  // Initialize digit control pins and set them to LOW (off)
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(lapButtonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(startButtonPin), interruptStartPause, FALLING);
  attachInterrupt(digitalPinToInterrupt(lapButtonPin), interruptLap, FALLING);

  // Begin serial communication for debugging purposes
  Serial.begin(9600);
}

void loop() {
  inputAction();
  if (counting) {
    incrementCounter();
  }
  else {
    writeNumber(number);
  }
}

void inputAction() {
  // Check if a possible button press was detected
  if(startPausePossiblePress) {

    if (startPauseShouldStartCounter) {
      startPauseLastDebounceTime = millis();
      startPauseShouldStartCounter = false;
    }
    if ((millis() - startPauseLastDebounceTime) > debounceDelay) {
      startPauseReading = digitalRead(startButtonPin);
      if (startPauseReading != startPauseState) {
        if (startPauseReading == LOW) {
          // START / PAUSE
          if (!counting) {
            counting = true;
          }
          else {
            paused = !paused;
          }
        }
      }
      startPausePossiblePress = false;
    }
  }

   resetReading = digitalRead(resetButtonPin);
   if (resetReading != lastResetState) {
     resetLastDebounceTime = millis();
   }
   if ((millis() - resetLastDebounceTime) > debounceDelay) {
     if (resetReading != resetState) {
       resetState = resetReading;
       if (resetState == LOW) {
         // RESET
         if (!counting) {
           for (int i=1; i<=lapMemorySize; i++) {
             lapMemory[i] = -1;
           }
           viewing = false;
           number = 0;
           lapCyclePosition = 0;
           lapSavingPosition = 1;
         }
         if (paused) {
           counting = false;
           paused = false;
           number = 0;
           lapCyclePosition = 0;
         }
       }
     }
   }
   lastResetState = resetReading;
  
  if(lapPossiblePress || lapPossibleLongPress) {
    if (lapShouldStartCounter) {
      lapLastDebounceTime = millis();
      lapShouldStartCounter = false;
    }
    if ((millis() - lapLastDebounceTime) > debounceDelay && lapPossiblePress) {
      lapReading = digitalRead(lapButtonPin);
      if (lapReading != lapState) {
        if (lapReading == LOW) {
          // LAP
          if (counting) {
            if (lapSavingPosition <= lapMemorySize) {
              lapMemory[lapSavingPosition] = number;
              lapSavingPosition++;
            }
            else {
              for (int i=1; i<lapMemorySize; i++) {
                lapMemory[i] = lapMemory[i+1];
              }
              lapMemory[lapMemorySize] = number;
            }
          }
          else{
            viewing = true;
            lapCyclePosition++;
            if (lapCyclePosition > lapMemorySize || lapMemory[lapCyclePosition] == -1) {
              lapCyclePosition = 0;
              viewing = false;
            }
            number = lapMemory[lapCyclePosition]; 
          }
        }
      }  
      lapPossiblePress = false;
    }
    if (millis() - lapLastDebounceTime > lapCycleDelay && lapPossibleLongPress) {
      lapReading = digitalRead(lapButtonPin);
      if (lapReading != lapState) {
        if (lapReading == LOW) {
          // LAP
          if (!counting) {
            viewing = true;
            lapCyclePosition++;
            if (lapCyclePosition > lapMemorySize || lapMemory[lapCyclePosition] == -1) {
              lapCyclePosition = 0;
              viewing = false;
            }
            number = lapMemory[lapCyclePosition]; 
          }
          endLapCycle = true;
          lapLastDebounceTime = millis();
        }
      }
      if (lapReading != LOW && endLapCycle)
        lapPossibleLongPress = false;
    }
  }
}

void incrementCounter() {
  if (!paused) {
    if (millis() - lastIncrement > delayCount) {
      number++;
      number %= 10000;  
      lastIncrement = millis();
    }
  }
  writeNumber(number);
}

void writeNumber(int number) {
  int currentNumber = number;
  int displayDigit = 3;  
  int lastDigit = 0;
  // Loop through each display digit
  while (displayDigit >= 0) {
    // Extract the last digit of the current number
    lastDigit = currentNumber % 10;
    // Activate the current digit on the display
    activateDisplay(displayDigit);
    // Output the byte encoding for the last digit to the display
    writeReg(byteEncodings[lastDigit] + ((displayDigit == decimalPointPosition)?1:0));
    // Move to the next digit
    displayDigit--;
    // Update 'currentNumber' by removing the last digit
    currentNumber /= 10;
    // Clear the display to prevent ghosting between digit activations
    writeReg(B00000000);  
  }
}

void activateDisplay(int displayNumber) {
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeReg(int digit) {
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}

void interruptStartPause() {
  if (!viewing) {
    startPausePossiblePress = true; // Indicate that a button press might have occurred
    startPauseShouldStartCounter = true; // Indicate the need to start the debounce counter
  }
}

void interruptLap() {
  if (!paused) {
    lapPossiblePress = true; // Indicate that a button press might have occurred
    lapShouldStartCounter = true; // Indicate the need to start the debounce counter
    lapPossibleLongPress = true; // Indicate that a long button press might have occurred
    endLapCycle = false; 
  }
}
