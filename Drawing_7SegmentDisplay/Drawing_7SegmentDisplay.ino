const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output
int xValue = 0; // vertical axis
int yValue = 0; // horizontal axis

// Declare all the segments pins
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;
const int segSize = 8;
int segments[segSize] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};

// joystick flags
int center = 1;
int verticalMoving = 0;
int horizontalMoving = 0;


//                    0, 1, 2, 3, 4, 5, 6, 7    indexes
//                    a, b, c, d, e, f, g, DP   LEDs of the 7-digit-display
int state[segSize] = {0, 0, 0, 0, 0, 0, 0, 0};
int blinkState[segSize] = {0, 0, 0, 0, 0, 0, 0, 0};
int position = 7; 
int blinkPosition = 7;

const int readingMinThreshold = 350;
const int readingMaxThreshold = 650;
const int resetMinThreshold = 450;
const int resetMaxThreshold = 550;
const int centerReferenceValue = 500;

const int blinkInterval = 200;
unsigned long lastBlinkToggleTime = 0;
unsigned long currentBlinkToggleTime = 0;

const unsigned long debounceDelay = 20;
const unsigned long longPushDuration = 2500;
volatile bool possiblePress = false; // Indicates that a button press might have occurred
volatile bool possibleLongPress = false; // Indicates that a long button press might have occurred
volatile bool shouldStartCounter = false; // Indicates the need to start debounce counter
bool ledState = LOW; // Current state of the LED (ON/OFF)
bool buttonState = HIGH; // The previous state of the button (HIGH when not pressed due to INPUT_PULLUP)
bool buttonReading = HIGH; // The current reading from the button pin
unsigned long lastDebounceTime = 0; // Keeps track of the last time the button was pressed

void setup() {
  pinMode(pinSW, INPUT_PULLUP); // activate pull-up resistor on the // push-button pin
  // Start the serial communication.
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  for (int i = 0; i < segSize; i++) {
    state[i] = 0;
    blinkState[i] = 0;
  }
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, FALLING);
}

void loop() {
  inputDetection();
  moveAction();
  updateBlink(position);
}



void inputDetection() {
  // Check if a possible button press was detected
  if(possiblePress || possibleLongPress) {
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
          state[position] = !state[position];
        }
      }  // If the states match, it was a false alarm due to noise
      // Reset the possiblePress flag
      possiblePress = false;
    }
    if (millis() - lastDebounceTime > longPushDuration) {
      buttonReading = digitalRead(pinSW);
      if (buttonReading != buttonState) {
        // Reset the display if the button was truly long-pressed (read as LOW due to INPUT_PULLUP)
        if (buttonReading == LOW) {
          Serial.println(6666);
          for (int i = 0; i < segSize; i++) {
            state[i] = 0;
            blinkState[i] = state[i];
          }
          position = 7;
        }
      }  // If the states match, it was a false alarm due to noise
      // Reset the possibleLongPress flag
      possibleLongPress = false;
    }

  }

  // Directional readings:
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  // Checking if the values read are past the thresholds and selecting the 'highest' value 
  // (if we move the joystick diagonally, we want to know if we went more towards one side, i.e. the North Northwest direction is more north than west)
  if (xValue > readingMaxThreshold && abs(xValue-centerReferenceValue) > abs(yValue-centerReferenceValue) && center == 1) {
    verticalMoving = 1;
    center = 0;
  }
  if (xValue < readingMinThreshold && abs(xValue-centerReferenceValue) > abs(yValue-centerReferenceValue) && center == 1) {
    verticalMoving = -1;
    center = 0;
  }
  if (yValue > readingMaxThreshold && abs(yValue-centerReferenceValue) > abs(xValue-centerReferenceValue) && center == 1) {
    horizontalMoving = 1;
    center = 0;
  }
  if (yValue < readingMinThreshold && abs(yValue-centerReferenceValue) > abs(xValue-centerReferenceValue) && center == 1) {
    horizontalMoving = -1;
    center = 0;
  }

  // If we read a center-like value (within a threshold interval), we must reset the center flag
  // Its purpose is to help us read only one input at a time after a joystcik move towards one extremity
  if (xValue >= readingMinThreshold && xValue <= readingMaxThreshold && yValue >= readingMinThreshold && yValue <= readingMaxThreshold) {
    center = 1;
  }
  return;
}



void moveAction() {
  switch (position) { // current position is:
  case 0: // a
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      position = 1; // go to b
      break;
    case -1: // LEFT input:
      position = 5; // go to f
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        // N/A
        break; 
      case -1: // DOWN input:
        position = 6; // go to g
         break;
       }
      break; // break horizontal 0 case
    }
    break; // break position 0 case

  case 1: // b
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      // N/A
      break;
    case -1: // LEFT input:
      position = 5; // go to f
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        position = 0; // go to a
        break; 
      case -1: // DOWN input:
        position = 6; // go to g
        break;
      }
      break; // break horizontal 0 case
    }
    break; // break position 1 case

  case 2: // c
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      position = 7; // go to dp
      break;
    case -1: // LEFT input:
      position = 4; // go to e
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        position = 6; // go to g
        break; 
      case -1: // DOWN input:
        position = 3; // go to d
        break;
      }
      break; // break horizontal 0 case
    }
    break; // break position 2 case

  case 3: // d
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      position = 2; // go to c
      break;
    case -1: // LEFT input:
      position = 4; // go to e
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        position = 6; // go to g
        break; 
      case -1: // DOWN input:
        // N/A
        break;
      }
      break; // break horizontal 0 case
    }
    break; // break position 3 case

  case 4: // e
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      position = 2; // go to c
      break;
    case -1: // LEFT input:
      // N/A
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        position = 6; // go to g
        break; 
      case -1: // DOWN input:
        position = 3; // go to d
        break;
      }
      break; // break horizontal 0 case
    }
    break; // break position 4 case

  case 5: // f
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      position = 1; // go to b
      break;
    case -1: // LEFT input:
      // N/A
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        position = 0; // go to a
        break; 
      case -1: // DOWN input:
        position = 6; // go to g
        break;
      }
      break; // break horizontal 0 case
    }
    break; // break position 5 case

  case 6: // g
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      // N/A
      break;
    case -1: // LEFT input:
      // N/A
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        position = 0; // go to a
        break; 
      case -1: // DOWN input:
        position = 3; // go to d
        break;
      }
      break; // break horizontal 0 case
    }
    break; // break position 6 case

  case 7: // dp
    switch (horizontalMoving) {
    case 1: // RIGHT input:
      // N/A
      break;
    case -1: // LEFT input:
        position = 2; // go to c
      break;
    case 0: // wasn't a horizontal input:
      switch (verticalMoving) {
      case 1: // UP input:
        // N/A
        break; 
      case -1: // DOWN input:
        // N/A
        break;
      }
      break; // break horizontal 0 case
    }
    break; // break position 7 case

  
  }

  if (position != blinkPosition) {
    lastBlinkToggleTime = millis();
    blinkState[blinkPosition] = state[blinkPosition]; // Set the old blinking led to the state it should be underneath
  }
  verticalMoving = 0;
  horizontalMoving = 0;
  // We've calculated the move, we can now reset the reading flags
}


void updateBlink(int newBlinkPosition) {
  currentBlinkToggleTime = millis();
  if (currentBlinkToggleTime - lastBlinkToggleTime > blinkInterval) {
    lastBlinkToggleTime = currentBlinkToggleTime; 
    blinkState[newBlinkPosition] = !blinkState[newBlinkPosition];
  }
  blinkPosition = newBlinkPosition;
  for (int i = 0; i < segSize; i++) {
    digitalWrite(segments[i], blinkState[i]);
  }
  return;
}

void writeState() {
  for (int i = 0; i < segSize; i++) {
    digitalWrite(segments[i], state[i]); 
  }
}


// Interrupt service routine (ISR) executed when the button is pressed
void handleInterrupt() {
  possiblePress = true; // Indicate that a button press might have occurred
  possibleLongPress = true; // Indicate that a long button press might have occurred
  shouldStartCounter = true; // Indicate the need to start the debounce counter
}

