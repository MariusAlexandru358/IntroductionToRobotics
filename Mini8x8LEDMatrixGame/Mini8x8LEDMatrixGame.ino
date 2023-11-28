#include "LedControl.h"  // Include LedControl library for controlling the LED matrix
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte xPin = A0; // A0 - analog pin connected to X output
const byte yPin = A1; // A1 - analog pin connected to Y output
const byte pinSW = 2; // digital pin connected to switch output
const byte ledPin = 6; // Pin for the information LED

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // LedControl object to manage the LED matrix
                            // DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2; // Variable to set the brightness level of the matrix

// Variables to track the current and previous positions of the joystick-controlled LED
byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

// Thresholds for detecting joystick movement
const short minThreshold = 300;
const short maxThreshold = 700;
const byte moveInterval = 200;     // Timing variable to control the speed of LED movement
unsigned long long lastMoved = 0;  // Tracks the last time the LED moved

const byte matrixSize = 8;         // Size of the LED matrix
bool matrixChanged = true;         // Flag to track if the matrix display needs updating

byte matrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
}; // 2D array representing the state of each LED (on/off) in the matrix

byte matrixByte[matrixSize] = {
  B00000000,
  B01000100,
  B00101000,
  B00010000,
  B00010000,
  B00010000,
  B00000000,
  B00000000
}; // Array representing each row of the LED matrix as a byte
const short startPositionSize = 1; // Size of the start position square, located in the top left corner. Recommended values: 0, 1 or 2

bool blinkState = 1;
const unsigned short blinkInterval = 500;
unsigned long lastBlinkToggleTime = 0;
unsigned long currentBlinkToggleTime = 0;

const unsigned short debounceDelay = 20;
const unsigned short mediumPushDuration = 2000;
const unsigned short longPushDuration = 4000;
volatile bool possiblePress = false; // Indicates that a button press might have occurred
volatile bool possibleMediumPress = false; // Indicates that a medium button press might have occurred
volatile bool possibleLongPress = false; // Indicates that a long button press might have occurred
volatile bool shouldStartCounter = false; // Indicates the need to start debounce counter
bool ledState = LOW; // Current state of the LED (ON/OFF)
bool buttonState = HIGH; // The previous state of the button (HIGH when not pressed due to INPUT_PULLUP)
bool buttonReading = HIGH; // The current reading from the button pin
unsigned long lastDebounceTime = 0; // Keeps track of the last time the button was pressed

// Bomb related variables
bool bomb = false; // Bomb flag: Is there a bomb?
byte bombX = 0; // Bomb X coordinate
byte bombY = 0; // Bomb Y coordinate
bool bombBlinkState = 1;
const unsigned short bombBlinkInterval = 50; // Bomb blink interval: very fast blinking (milliseconds)
unsigned long lastBombBlinktoggleTime = 0; // Used for timing the bomb blinks 
byte bombSize = 0; // Bomb size (small / medium / big)
const byte smallBombSize = 1; // Value for the small bomb
const byte mediumBombSize = 2; // Value for the medium bomb
const byte bigBombSize = 3; // Value for the big bomb
// bool smallBomb = false; // Small bomb flag
// bool mediumBomb = false; // Medium bomb flag
// bool bigBomb = false; // Big bomb flag
const unsigned short bombFuseDuration = 2500; // Time until explosion from when the bomb was placed (milliseconds)
unsigned long bombPlacementTime = 0; // Timestamp of when the bomb was placed
bool exploding = false;
const unsigned short explosionDuration = 50; // Time it takes for the explosion to happen and to then clear out
unsigned long explosionStartTime = 0; // Timestamp of when the explosion started
const byte bombBaselineIntensity = 50; // Baseline instensity for the information LED (higher = bigger bomb)

bool gameOver = false;

void setup() {
  // Serial.begin(9600);
  pinMode(pinSW, INPUT_PULLUP); // activate pull-up resistor on the // push-button pin
  pinMode(ledPin , OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, FALLING);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // Clear the matrix display
  
  
  randomSeed(analogRead(A5)); // Initialize random seed

  generateMap();

}

void loop() {
  // updateByteMatrix();
  if (millis() - lastMoved > moveInterval) {  // Check if it's time to move the LED
    // game logic
    updatePositions();     // Update the position of the LED based on joystick input
    lastMoved = millis();  // Update the time of the last move
  }


  if(possiblePress || possibleMediumPress || possibleLongPress) {
    // Start the debounce counter if needed
    if (shouldStartCounter) {
      lastDebounceTime = millis();
      shouldStartCounter = false;
    }
    // If the debounce period has elapsed
    if ((millis() - lastDebounceTime) > debounceDelay && possiblePress) {
      // Read the current state of the button
      buttonReading = digitalRead(pinSW);
      // If the button state has changed (i.e., if it was not a false press)
      if (buttonReading != buttonState) {
        // Toggle the LED state if the button was truly pressed (read as LOW due to INPUT_PULLUP)
        if (buttonReading == LOW) {
          // SMALL BOMB (small plus shape)
          if (!bomb || (bombX == xPos && bombY == yPos) )
            initializeBombPlacement(smallBombSize);
        }
      }  // If the states match, it was a false alarm due to noise
      // Reset the possiblePress flag
      possiblePress = false;
    }
    if ((millis() - lastDebounceTime) > mediumPushDuration && possibleMediumPress) {
      // Read the current state of the button
      buttonReading = digitalRead(pinSW);
      // If the button state has changed (i.e., if it was not a false press)
      if (buttonReading != buttonState) {
        // Toggle the LED state if the button was truly pressed (read as LOW due to INPUT_PULLUP)
        if (buttonReading == LOW) {
          // MEDIUM BOMB (3x3 square shape)
          if (!bomb || (bombX == xPos && bombY == yPos) )
            initializeBombPlacement(mediumBombSize);
        }
      }  // If the states match, it was a false alarm due to noise
      // Reset the possibleMeidumPress flag
      possibleMediumPress = false;
    }
    if (millis() - lastDebounceTime > longPushDuration) {
      buttonReading = digitalRead(pinSW);
      if (buttonReading != buttonState) {
        // Reset the display if the button was truly long-pressed (read as LOW due to INPUT_PULLUP)
        if (buttonReading == LOW) {
          // BIG BOMB (5x5 rectangle with round corners shape)
          if (!bomb || (bombX == xPos && bombY == yPos) )
            initializeBombPlacement(bigBombSize);
        }
      }  // If the states match, it was a false alarm due to noise
      // Reset the possibleLongPress flag
      possibleLongPress = false;
    }

  }

  if (millis() - lastBlinkToggleTime > blinkInterval) { // player blinking
    blinkState = !blinkState;
    // matrix[xPos][yPos] = blinkState;
    lc.setLed(0, xPos, yPos, blinkState);
    lastBlinkToggleTime = millis();
  }

  if (bomb) {
   if (millis() - lastBombBlinktoggleTime > bombBlinkInterval) { // bomb blinking
    bombBlinkState = !bombBlinkState;
    // matrix[bombX][bombY] = !matrix[bombX][bombY];
    lc.setLed(0, bombX, bombY, bombBlinkState);
    lastBombBlinktoggleTime = millis();
   }
   if (millis() - bombPlacementTime > bombFuseDuration) {
    explodeBomb(); // TODO
   }
  }

  if (matrixChanged == true) {  // Check if the matrix display needs updating
    updateMatrix();             // Update the LED matrix display
    matrixChanged = false;      // Reset the update flag
  }

  gameOver = checkGameOver();
  if (gameOver)
  {
    generateMap();
    gameOver = false;
  }
}



void generateMap() {
  // Calculate the number of true values to fill between 60% and 85% of the matrix
  int minTrueCount = 0.5 * matrixSize * matrixSize;
  int maxTrueCount = 0.75 * matrixSize * matrixSize;
  int trueCount = random(minTrueCount, maxTrueCount + 1) - pow(2, startPositionSize);

  // Set true values randomly
  for (int count = 0; count < trueCount; ++count) {
    int row = random(matrixSize);
    int col = random(matrixSize);

    if (row <= xPos+startPositionSize && row >= xPos-startPositionSize && col <= yPos+startPositionSize && col>= yPos-startPositionSize) { // Design choice to not generate walls in the start position (top left corner)
      --count;
      continue;
    }
    // Check if the element is already true, if not, set it to true
    if (!matrix[row][col]) {
      matrix[row][col] = true;
    } else {
      // If the element is already true, try again
      --count;
    }
  }
  matrix[xPos][yPos] = 1; // Initialize the starting position
  updateMatrix();
}

void initializeBombPlacement(byte bombType) {
  bomb = true;
  bombX = xPos;
  bombY = yPos;
  bombSize = bombType;
  bombPlacementTime = millis();
  lastBombBlinktoggleTime = bombPlacementTime;
  analogWrite(ledPin, bombBaselineIntensity*bombType);  
}

void explodeBomb() {
  // TODO
  // explode and clear bomb radius
  // reset bomb params
  if (!exploding) {
    if (bombSize == smallBombSize) {
      lc.setLed(0, bombX, bombY, true);
      lc.setLed(0, bombX+bombSize, bombY, true);
      lc.setLed(0, bombX-bombSize, bombY, true);
      lc.setLed(0, bombX, bombY+bombSize, true);
      lc.setLed(0, bombX, bombY-bombSize, true);
    }
    else if (bombSize == mediumBombSize) {
      for (int i=bombX-bombSize+1; i<bombX+bombSize; ++i)
        for (int j=bombY-bombSize+1; j<bombY+bombSize; ++j)
          lc.setLed(0, i, j, true);
      
    }
    else if (bombSize == bigBombSize) {
      for (int i=bombX-bombSize+1; i<bombX+bombSize; ++i)
        for (int j=bombY-bombSize+1; j<bombY+bombSize; ++j)
        {
          if (abs(i-bombX) >= bombSize-1 && abs(j-bombY) >= bombSize-1)
          {
            continue;
          }  
          lc.setLed(0, i, j, true);
        }
      // lc.setLed(0, bombX-bombSize, bombY-bombSize, false);
      // lc.setLed(0, bombX-bombSize, bombY+bombSize, false);
      // lc.setLed(0, bombX+bombSize, bombY-bombSize, false);
      // lc.setLed(0, bombX+bombSize, bombY+bombSize, false);
    }
    exploding = true;
    explosionStartTime = millis();
    possiblePress = false;
    possibleMediumPress = false;
    possibleLongPress = false;
    analogWrite(ledPin, 0);
  }
  else {
    if (millis() - explosionStartTime > explosionDuration) {
      if (bombSize == smallBombSize) {
        matrix[bombX][bombY] = 0;
        matrix[bombX+bombSize][bombY] = 0;
        matrix[bombX-bombSize][bombY] = 0;
        matrix[bombX][bombY+bombSize] = 0;
        matrix[bombX][bombY-bombSize] = 0;
        lc.setLed(0, bombX, bombY, false);
        lc.setLed(0, bombX+bombSize, bombY, false);
        lc.setLed(0, bombX-bombSize, bombY, false);
        lc.setLed(0, bombX, bombY+bombSize, false);
        lc.setLed(0, bombX, bombY-bombSize, false);
      } 
      else if (bombSize == mediumBombSize) {
        for (int i=bombX-bombSize+1; i<bombX+bombSize; ++i)
          for (int j=bombY-bombSize+1; j<bombY+bombSize; ++j) 
          {
            lc.setLed(0, i, j, false);
            matrix[i][j] = 0;
          }
      }
      else if (bombSize == bigBombSize) {
        for (int i=bombX-bombSize+1; i<bombX+bombSize; ++i)
          for (int j=bombY-bombSize+1; j<bombY+bombSize; ++j)
          {
            if (abs(i-bombX) >= bombSize-1 && abs(j-bombY) >= bombSize-1) 
            {
              continue;
            }
            // lc.setLed(0, i, j, false);
            matrix[i][j] = 0;
          }
        matrixChanged = true;
      }
      exploding = false;
      bomb = false;
      bombSize = 0;
    }
    
  }

  
}


bool checkGameOver() {
  int sum = 0;
  for (int i=0; i<matrixSize; ++i) 
    for (int j=0; j<matrixSize; ++j) 
    {
      sum += matrix[i][j];
      if (sum > 1)
        return false;
    }
  return true;
}



void updateByteMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, matrixByte[row]);  // set each ROW (or COL) at the same time
  }
}

void updateMatrix() { // update the display
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);  // set each led individually
    }
  }
}

// Function to read joystick input and update the position of the LED
void updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  // Store the last positions of the LED
  xLastPos = xPos;
  yLastPos = yPos;
  // Update xPos based on joystick movement (X-axis)
  if (xValue < minThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    } else {
      // xPos = 0;
    }
  }
  if (xValue > maxThreshold) {
    if (xPos > 0) {
      xPos--;
    } else {
      // xPos = matrixSize - 1;
    }
  }
  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    } else {
      // yPos = 0;
    }
  }
  // Update xPos based on joystick movement (Y-axis)
  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
    } else {
      // yPos = matrixSize - 1;
    }
  }
  // Check if the position has changed and update the matrix if necessary
  if (matrix[xPos][yPos] == 1) {
    xPos = xLastPos;
    yPos = yLastPos;
  }
  if (xPos != xLastPos || yPos != yLastPos) {
    // matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
    lc.setLed(0, xLastPos, yLastPos, false);
    lc.setLed(0, xPos, yPos, true);
  }
}









void handleInterrupt() {
  possiblePress = true; // Indicate that a button press might have occurred
  possibleMediumPress = true; // Indicate that a medium button press might have occurred
  possibleLongPress = true; // Indicate that a long button press might have occurred
  shouldStartCounter = true; // Indicate the need to start the debounce counter
}



