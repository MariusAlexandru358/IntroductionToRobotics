#include <EEPROM.h>
#include <LiquidCrystal.h>
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const byte lcdIntensityPin = 3;
#define LCD_L 16


#include "LedControl.h"  // Include LedControl library for controlling the LED matrix
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte xPin = A0; // A0 - analog pin connected to X output
const byte yPin = A1; // A1 - analog pin connected to Y output
const byte pinSW = 2; // digital pin connected to switch output
const byte ledPin = A5; // Pin for the information LED

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // LedControl object to manage the LED matrix




// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- BOMB GAME -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


byte matrixBrightness = 2; // Variable to set the brightness level of the matrix

// Variables to track the current and previous positions of the joystick-controlled LED
byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

// Thresholds for detecting joystick movement
// const short minThreshold = 300;
// const short maxThreshold = 700;
// const byte moveInterval = 200;     // Timing variable to control the speed of LED movement
// unsigned long long lastMoved = 0;  // Tracks the last time the LED moved

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
const float minFillPercentage = 0.5;
const float maxFillPercentage = 0.75;
const short startPositionSize = 1; // Size of the start position square, located in the top left corner. Recommended values: 0, 1 or 2

bool blinkState = 1;
const unsigned short blinkInterval = 500;
unsigned long lastBlinkToggleTime = 0;
unsigned long currentBlinkToggleTime = 0;

// const unsigned short debounceDelay = 20;
const unsigned short mediumPushDuration = 2000;
// const unsigned short longPushDuration = 4000;
// volatile bool possiblePress = false; // Indicates that a button press might have occurred
volatile bool possibleMediumPress = false; // Indicates that a medium button press might have occurred
// volatile bool possibleLongPress = false; // Indicates that a long button press might have occurred
// volatile bool shouldStartCounter = false; // Indicates the need to start debounce counter
// bool ledState = LOW; // Current state of the LED (ON/OFF)
// bool buttonState = HIGH; // The previous state of the button (HIGH when not pressed due to INPUT_PULLUP)
// bool buttonReading = HIGH; // The current reading from the button pin
// unsigned long lastDebounceTime = 0; // Keeps track of the last time the button was pressed

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

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

bool paused = false;
unsigned short number = 0; // In-game timer
unsigned long lastIncrement = 0;
const unsigned short delayCount = 1000;



// const byte pinSW = 2; // digital pin connected to switch output
const byte pinX = A0; // A0 - analog pin connected to X output
const byte pinY = A1; // A1 - analog pin connected to Y output
short xValue = 0; // vertical axis
short yValue = 0; // horizontal axis
// joystick flags
short center = 1;
short verticalMoving = 0;
short horizontalMoving = 0;

// const short readingMinThreshold = 300;
// const short readingMaxThreshold = 700;
// const short resetMinThreshold = 450;
// const short resetMaxThreshold = 550;
// const short centerReferenceValue = 500;

#define readingMinThreshold 300
#define readingMaxThreshold 700
#define resetMinThreshold 450
#define resetMaxThreshold 550
#define centerReferenceValue 500
const byte moveInterval = 200;     // Timing variable to control the speed of LED movement
unsigned long lastMoved = 0;  // Tracks the last time the LED moved


const unsigned long debounceDelay = 20;
const unsigned long longPushDuration = 2500;
volatile bool possiblePress = false; // Indicates that a button press might have occurred
volatile bool possibleLongPress = false; // Indicates that a long button press might have occurred
volatile bool shouldStartCounter = false; // Indicates the need to start debounce counter
bool ledState = LOW; // Current state of the LED (ON/OFF)
bool buttonState = HIGH; // The previous state of the button (HIGH when not pressed due to INPUT_PULLUP)
bool buttonReading = HIGH; // The current reading from the button pin
unsigned long lastDebounceTime = 0; // Keeps track of the last time the button was pressed


#define INTRO_MESSAGE_CODE 0
#define MAIN_MENU_CODE 1
#define HIGHSCORES_CODE 2
#define ABOUT_CODE 3
#define SETTINGS_CODE 10
#define LCD_BRIGHTNESS_SETTINGS_CODE 11
#define MATRIX_BRIGHTNESS_SETTINGS_CODE 12
#define SOUND_SETTINGS_CODE 13
#define END_MESSAGE_CODE 20
#define GAMEPLAY_CODE 30

#define INPUT_UP 'u'
#define INPUT_DOWN 'd'
#define INPUT_LEFT 'l'
#define INPUT_RIGHT 'r'
#define INPUT_CLICK 'c'
#define INPUT_LONG_CLICK 'k'

#define MENU_OPTION_1 1
#define MENU_OPTION_2 2
#define MENU_OPTION_3 3
#define MENU_OPTION_4 4
#define MENU_OPTION_5 5
#define MENU_OPTION_BACK 47808 // hex BAC0 :)

#define EEPROM_HIGHSCORE_ADDRESS 0 //TODO CONST BYTE ?
#define MAX_HIGHSCORES_SAVED 3
#define MAX_HIGHSCORE_NAME_LENGTH 3

#define EEPROM_LCD_BRIGHTNESS_ADDRESS 16
#define EEPROM_MATRIX_BRIGHTNESS_ADDRESS 18

byte n = 10;
byte distanceReadings[10];
unsigned short int lightReadings[10];
byte distanceIndex = 0;
byte lightIndex = 0;
byte distanceIndexEEPROM = 10;
byte lightIndexEEPROM = 20;
unsigned short int currentValueEEPROM = 0;


unsigned long IntroMessageStartTimestamp = 0;
const short IntroMessageDuration = 2000;
unsigned long lastCursorToggleTime = 0;
const short cursorToggleInterval = 500;
// -=-=-=-
byte menuPosition = MENU_OPTION_1;
char *mainMenuOptions[] = {"1.Play", "2.Highscore", "3.Settings", "4.About", "5.How to play"};
const byte mainMenuSize = 5; //TODO #DEFINE ?
char *settingsMenuOptions[] = {"1.LCD Bright.", "2.Matr Bright.", "3.Sound"}; //BONUS ADD SCROLLING EFFECT LIKE 314PRINTERS
const byte settingsMenuSize = 3; 

// -=-=-=-
const char lcdGameName[] = "  Minesweeper!  ";
const String lcdAuthor = "                Alexandru Marius  github.com/MariusAlexandru358                ";
unsigned long lastAboutScrollTime = 0;
const short scrollInterval = 500; // Time interval for scrolling (in milliseconds)
byte scrollPosition = 0;

byte highscores[MAX_HIGHSCORES_SAVED+1][MAX_HIGHSCORE_NAME_LENGTH+1];
byte highscoresSaved = 0;

byte stateControl = 0;
bool startedIntroMessage = false;
bool playingIntroMessage = false;



// -=-=-=-
unsigned short input;
byte brightnessSetting = 9;
byte matrBrightnessSetting = 2;
bool cursor = false;








void setup() {
  pinMode(lcdIntensityPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, FALLING);  
  lcd.begin(16,2);
  lcd.clear();
  highscoresSaved = EEPROM.read(EEPROM_HIGHSCORE_ADDRESS);
  for (short i=0; i<highscoresSaved; ++i) {
    for (short j=0; j<=MAX_HIGHSCORE_NAME_LENGTH; ++j) {
      highscores[i][j] = EEPROM.read(EEPROM_HIGHSCORE_ADDRESS+1+i+j);
    }
  }

  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.clearDisplay(0);                    // Clear the matrix display  

  // EEPROM.update(EEPROM_MATRIX_BRIGHTNESS_ADDRESS, 2);
  matrBrightnessSetting = EEPROM.read(EEPROM_MATRIX_BRIGHTNESS_ADDRESS);
  lc.setIntensity(0, matrBrightnessSetting);  // sets matrix brightness (0~15 possible values)

  // EEPROM.update(EEPROM_LCD_BRIGHTNESS_ADDRESS, 9);
  brightnessSetting = EEPROM.read(EEPROM_LCD_BRIGHTNESS_ADDRESS);
  byte lcdIntensityVal = map(brightnessSetting, 0, 9, 55, 255);
  analogWrite(lcdIntensityPin, lcdIntensityVal);

  Serial.begin(9600);
  // printMainMenu(MENU_OPTION_1);

  // stateControl = MAIN_MENU_CODE;
  // printMainMenu(MENU_OPTION_1);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  randomSeed(analogRead(A4)); // Initialize random seed

  generateMap();
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
}


void loop() {
  switch (stateControl) {
  case INTRO_MESSAGE_CODE:
    IntroMessage(); // TOTEST
    break;
  case MAIN_MENU_CODE:
    MainMenu(); 
    break;
  case SETTINGS_CODE:
    SettingsMenu();
    break;
  case LCD_BRIGHTNESS_SETTINGS_CODE:
    LCDBrightnessMenu();
    break;
  case MATRIX_BRIGHTNESS_SETTINGS_CODE:
    MatrixBrightnessMenu();
    break;
  case SOUND_SETTINGS_CODE:
    SoundMenu();
    break; 
  case ABOUT_CODE:
    AboutMenu();
    break;
  case END_MESSAGE_CODE:
    EndMessage(); //TODO
    break;
  case GAMEPLAY_CODE:
    Gameplay(); //TODO
    break;
  case HIGHSCORES_CODE:
    HighscoreMenu(); //TODO
    break;
  case 40: // TOTEST EXPAND
    
    break;
  default:
    MainMenu();
    break;
  }

  // handleSensors();
  // RGBLED();
}


void IntroMessage() { // TODO declare vars // TOTEST probabli nu merge
  if (!startedIntroMessage && !playingIntroMessage) {
    lcd.setCursor(3,0);
    lcd.print(F("Greetings!")); 
    lcd.setCursor(17,0);
    lcd.print(F("Welcome to")); // probabil nu merge ^^ muta printuri noi dupa scroll?? nah, yt video?
    lcd.setCursor(16,1);
    lcd.print(F("Minesweeper!"));
    startedIntroMessage = true;
    IntroMessageStartTimestamp = millis();
  }
  if (startedIntroMessage && millis() - IntroMessageStartTimestamp > IntroMessageDuration) {
    for (short position=0; position<14; ++position) {
      lcd.scrollDisplayLeft();
      delayMicroseconds(10);
    }
    startedIntroMessage = false;
    playingIntroMessage = true;
    IntroMessageStartTimestamp = millis();
  }
  if (playingIntroMessage && millis() - IntroMessageStartTimestamp > IntroMessageDuration) {
    lcd.clear();
    // stateControl = MAIN_MENU_CODE;
    playingIntroMessage = false;
    stateControl = MAIN_MENU_CODE;
    printMainMenu(MENU_OPTION_1); //TODO & maybe clear lcd at the top of every printMenu() func
  }
}

void printMainMenu(byte currentOption) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(">"); //TODO CUSTOM CHAR
  lcd.setCursor(1,0);
  lcd.print(mainMenuOptions[currentOption-1]);
  if (currentOption != MENU_OPTION_1) {
    lcd.setCursor(15,0);
    lcd.print("^"); //TODO CUSTOM CHAR
  }
  if (currentOption != mainMenuSize) {
    lcd.setCursor(1,1);
    lcd.print(mainMenuOptions[currentOption]); //TODO CUSTOM CHAR
    lcd.setCursor(15,1);
    lcd.print("v"); //TODO CUSTOM CHAR
  }
}
void MainMenu() {
  input = detectInput(); 
  switch (input) {
  case INPUT_UP:
    if (menuPosition > MENU_OPTION_1) {
      menuPosition--;
      printMainMenu(menuPosition);
    }
    break;
  case INPUT_DOWN:
    if (menuPosition < mainMenuSize) {
      menuPosition++;
      printMainMenu(menuPosition);
    }
    break;
  case INPUT_LEFT:
    break;
  case INPUT_RIGHT:
    break;
  case INPUT_CLICK:
    MainMenuAction(menuPosition);
    break;
  default:
    break;  
  }
}
void MainMenuAction(short selectedOption) { 
  menuPosition = MENU_OPTION_1; //reset the menuPosition for the next time we enter a menu or submenu
  switch (selectedOption) {
  case MENU_OPTION_1: // Start game //TODO ANIMATION or MESSAGE
    stateControl = GAMEPLAY_CODE;
    Gameplay(); //TODO
    break;
  case MENU_OPTION_2: // Highscore //TODO ANIMATION //TODO ADD THE NAVIGABLE STATE --> DONE? */
    stateControl = HIGHSCORES_CODE; 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Highscore:")); // MAYBE make a func(lineIndex) to print these 
    for (short i=0; i<= MAX_HIGHSCORE_NAME_LENGTH; ++i) {
      lcd.setCursor(MAX_HIGHSCORE_NAME_LENGTH+i,1);
      lcd.write(highscores[0][i]);
    }
    HighscoreMenu(); //TODO AND TEST/THINK IF IT HAS TO BE PRINT;MENU;ACTION TOO
    // for (short i=1; i<=highscoresSaved; i+=MAX_HIGHSCORE_NAME_LENGTH+1) {
    //   lcd.setCursor(MAX_HIGHSCORE_NAME_LENGTH,1);
    //   for (short j=0; j<=MAX_HIGHSCORE_NAME_LENGTH; ++j) {
    //     chr = EEPROM.read(EEPROM_HIGHSCORE_ADDRESS+i+j);

    //   }
    // }
    break;
  case MENU_OPTION_3: // Settings
    stateControl = SETTINGS_CODE;
    printSettingsMenu(MENU_OPTION_1); 
    SettingsMenu();
    break;
  case MENU_OPTION_4: // About
    stateControl = ABOUT_CODE;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(lcdGameName);
    AboutMenu();
    // lcd.setCursor(0,1);
    break;
  case MENU_OPTION_5: // How to play

    break;
  default:
    break;
  }
}



void HighscoreMenu() {

}



void printSettingsMenu(byte currentOption) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(">"); //TODO CUSTOM CHAR
  lcd.setCursor(1,0);
  lcd.print(settingsMenuOptions[currentOption-1]);
  if (currentOption != MENU_OPTION_1) {
    lcd.setCursor(15,0);
    lcd.print("^"); //TODO CUSTOM CHAR
  }
  if (currentOption != settingsMenuSize) {
    lcd.setCursor(1,1);
    lcd.print(settingsMenuOptions[currentOption]); //TODO CUSTOM CHAR
    lcd.setCursor(15,1);
    lcd.print("v"); //TODO CUSTOM CHAR
  }
}
void SettingsMenu() {
  input = detectInput();
  switch (input) {
  case INPUT_UP:
    if (menuPosition > MENU_OPTION_1) {
      menuPosition--;
      printSettingsMenu(menuPosition);
    }
    break;
  case INPUT_DOWN:
    if (menuPosition < settingsMenuSize) {
      menuPosition++;
      printSettingsMenu(menuPosition);
    }
    break;
  case INPUT_LEFT:
    SettingsAction(MENU_OPTION_BACK);
    break;
  case INPUT_RIGHT:
    break;
  case INPUT_CLICK:
    SettingsAction(menuPosition);
    break;
  default:
    break;  
  }
}
void SettingsAction(short selectedOption) {
  menuPosition = MENU_OPTION_1; //reset the menuPosition for the next time we enter a menu or submenu
  switch (selectedOption) {
  case MENU_OPTION_1: // LCD brightness control
    stateControl = LCD_BRIGHTNESS_SETTINGS_CODE;
    menuPosition = brightnessSetting;
    printLCDBrightnessMenu(); 
    LCDBrightnessMenu(); 
    break;
  case MENU_OPTION_2: // Matrix brightness control
    stateControl = MATRIX_BRIGHTNESS_SETTINGS_CODE;
    menuPosition = matrBrightnessSetting-1;
    printMatrixBrightnessMenu(); //TOTEST
    MatrixBrightnessMenu(); //TODO STARTED
    break;
  case MENU_OPTION_3: // Sound on/off
    stateControl = SOUND_SETTINGS_CODE;
    printSoundMenu(); //TOTEST
    SoundMenu(); //TODO STARTED
    break;
  case MENU_OPTION_BACK:
    stateControl = MAIN_MENU_CODE;
    printMainMenu(MENU_OPTION_1);
    MainMenu();
    break;
  default:
    break;
  }
}

void printLCDBrightnessMenu() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print(F("^ LCD Brightness"));
  lcd.setCursor(1,1);
  lcd.print(F("< 0123456789 >"));
  lcd.setCursor(3+brightnessSetting,1);
  lcd.cursor(); 
}
void LCDBrightnessMenu() {
  byte val;
  unsigned long currentMillis;
  currentMillis = millis();
  if (currentMillis - lastCursorToggleTime >= cursorToggleInterval) {
    lastCursorToggleTime = currentMillis;
    if(cursor) {
      lcd.noCursor();
    }
    else {
      lcd.cursor();
    }  
    cursor = !cursor;
  }

  input = detectInput();
  switch (input) {
  case INPUT_UP:
    LCDBrightnessAction(MENU_OPTION_BACK);
    break;
  case INPUT_DOWN:
    break;

  case INPUT_LEFT:
    if (menuPosition)
      menuPosition--;
    // if (menuPosition = 0) {
    //   menuPosition = 0;
    //   Serial.println("LCD Brightness Options - lower limmit hit.");
    // }
    val = map(menuPosition, 0, 9, 56, 255);
    analogWrite(lcdIntensityPin, val);
    brightnessSetting = menuPosition;
    lcd.setCursor(3+menuPosition,1);
    break;

  case INPUT_RIGHT:
    menuPosition++;
    if (menuPosition > 9)
      menuPosition = 9;
    val = map(menuPosition, 0, 9, 56, 255);
    analogWrite(lcdIntensityPin, val);
    brightnessSetting = menuPosition;
    lcd.setCursor(3+menuPosition,1);
    break;
  
  case INPUT_CLICK:
    lcd.noCursor();
    cursor = false;
    LCDBrightnessAction(menuPosition);
    break;
  default:
    break;  
  }
}
void LCDBrightnessAction(short selectedOption) {
  switch (selectedOption) {
  case MENU_OPTION_BACK:
    EEPROM.update(EEPROM_LCD_BRIGHTNESS_ADDRESS, brightnessSetting);

    stateControl = SETTINGS_CODE;
    menuPosition = MENU_OPTION_1; //reset the menuPosition for the next time we enter a menu or submenu
    printSettingsMenu(MENU_OPTION_1);
    SettingsMenu();
    break;
  default:
    //TODO ANIMATION WHITE_CHECKAMRK FOR VISUAL FEEDBACK
    LCDBrightnessAction(MENU_OPTION_BACK);
    break;
  }  
}

void printMatrixBrightnessMenu() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print(F("^ Matrix Bright."));
  lcd.setCursor(1,1);
  lcd.print(F("< 0123456789 >"));
  lcd.setCursor(2+matrBrightnessSetting,1);
  lcd.cursor(); //TOTEST is an on/off cycle needed to blink?
}
void MatrixBrightnessMenu() {
  unsigned long currentMillis;
  input = detectInput();
  currentMillis = millis();
  if (currentMillis - lastCursorToggleTime >= cursorToggleInterval) {
    lastCursorToggleTime = currentMillis;
    if(cursor) {
      lcd.noCursor();
    }
    else {
      lcd.cursor();
    }  
    cursor = !cursor;
  }

  switch (input) {
  case INPUT_UP:
    MatrixBrightnessAction(MENU_OPTION_BACK);
    break;
  case INPUT_DOWN:
    break;

  case INPUT_LEFT:
    if (menuPosition)
      menuPosition--;
    if (menuPosition < 0)
      menuPosition = 0;
    // byte val = map(menuPosition, 0, 9, 1, 10);
    matrBrightnessSetting = menuPosition+1;
    lc.setIntensity(0, matrBrightnessSetting);  // sets matrix brightness (0~15 possible values)
    lcd.setCursor(3+menuPosition,1);
    break;

  case INPUT_RIGHT:
    menuPosition++;
    if (menuPosition > 9)
      menuPosition = 9;
    // byte val = map(menuPosition, 0, 9, 1, 10);
    matrBrightnessSetting = menuPosition+1;
    lc.setIntensity(0, matrBrightnessSetting);  // sets matrix brightness (0~15 possible values)
    lcd.setCursor(3+menuPosition,1);
    break;

  case INPUT_CLICK:
    lcd.noCursor();
    cursor = false;
    MatrixBrightnessAction(menuPosition);
    break;
  default:
    break;  
  }
}
void MatrixBrightnessAction(short selectedOption) {
  switch (selectedOption) {
  case MENU_OPTION_BACK:
    EEPROM.update(EEPROM_MATRIX_BRIGHTNESS_ADDRESS, matrBrightnessSetting);

    stateControl = SETTINGS_CODE;
    menuPosition = MENU_OPTION_1; //reset the menuPosition for the next time we enter a menu or submenu
    printSettingsMenu(MENU_OPTION_1);
    SettingsMenu();
    break;
  default:
    //TODO ANIMATION WHITE_CHECKAMRK FOR VISUAL FEEDBACK
    MatrixBrightnessAction(MENU_OPTION_BACK);
    break;
  }
}

void printSoundMenu() {

}
void SoundMenu() { //TODO
  stateControl = SETTINGS_CODE; // TODO (and delete this)
}




void AboutMenu() {
  input = detectInput();
  unsigned long currentMillis = millis();
  if (currentMillis - lastAboutScrollTime >= scrollInterval) {
    lastAboutScrollTime = currentMillis;

    lcd.setCursor(0, 1); // Set the cursor to the second line

    // Print the scrolling portion of the message
    lcd.print(lcdAuthor.substring(scrollPosition, scrollPosition + LCD_L));

    // Move to the next position for the next loop
    scrollPosition++;

    // Reset the scroll position when the end of the message is reached
    if (scrollPosition > lcdAuthor.length() - LCD_L) {
      scrollPosition = 0;
      lcd.clear(); // Clear the LCD before displaying the constant text again
      lcd.setCursor(0, 0);
      lcd.print(lcdGameName);
    }
  }
  
  switch (input) {
  case INPUT_LEFT:
    Serial.println("Back from About");
    stateControl = MAIN_MENU_CODE;
    printMainMenu(MENU_OPTION_1);
    MainMenu();
    break;
  case INPUT_UP:
    Serial.println("Back from About");
    stateControl = MAIN_MENU_CODE;
    printMainMenu(MENU_OPTION_1);
    MainMenu();
    break;
  default:
    break;
  }
}



void printEndMessage(byte score) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Congratulations!"));
  lcd.setCursor(0,1);
  lcd.print(F("Time: "));
  lcd.setCursor(6,1);
  lcd.print(score/100);
  lcd.setCursor(7,1);
  lcd.print((score/10)%10);
  lcd.setCursor(8,1);
  lcd.print(score%10);
  lcd.setCursor(10,1);
  lcd.print(F("s"));
}
void EndMessage() {
  input = detectInput();
  if (input) {
    stateControl = MAIN_MENU_CODE;
    printMainMenu(MENU_OPTION_1);
    return;
  }
}





unsigned char detectInput() {
  unsigned char returnedInput = 0;
  // Check if a possible button press was detected
  if (possiblePress || possibleLongPress) {
    
          Serial.println("possilbe click ?");
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
          Serial.println("click");
          returnedInput = INPUT_CLICK;
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
          Serial.println("long click");
          returnedInput = INPUT_LONG_CLICK;
        }
      }  // If the states match, it was a false alarm due to noise
      // Reset the possibleLongPress flag
      possibleLongPress = false;
    }
  }
  
  if (!returnedInput && millis() - lastMoved > moveInterval) {  // Check if it's time to move the LED
    // Directional readings:
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    // Checking if the values read are past the thresholds and selecting the 'highest' value
    // (if we move the joystick diagonally, we want to know if we went more towards one side, i.e. the North Northwest direction is more north than west)
    if (xValue > readingMaxThreshold && abs(xValue - centerReferenceValue) > abs(yValue - centerReferenceValue)) {
      returnedInput = INPUT_UP;
    }
    if (xValue < readingMinThreshold && abs(xValue - centerReferenceValue) > abs(yValue - centerReferenceValue)) {
      returnedInput = INPUT_DOWN;
    }
    if (yValue > readingMaxThreshold && abs(yValue - centerReferenceValue) > abs(xValue - centerReferenceValue)) {
      returnedInput = INPUT_RIGHT;
    }
    if (yValue < readingMinThreshold && abs(yValue - centerReferenceValue) > abs(xValue - centerReferenceValue)) {
      returnedInput = INPUT_LEFT;
    }
    
    lastMoved = millis();  // Update the time of the last move
  }
  if (returnedInput) {
    Serial.print((char)returnedInput);
    Serial.print("| ");
    Serial.print(INPUT_UP);
    Serial.print(INPUT_DOWN);
    Serial.print(INPUT_RIGHT);
    Serial.print(INPUT_LEFT);
    Serial.print(INPUT_CLICK);
    Serial.println(INPUT_LONG_CLICK);
  }
  return returnedInput;
}

void handleInterrupt() {
  Serial.println("ISR");
  possiblePress = true; // Indicate that a button press might have occurred
  possibleLongPress = true; // Indicate that a long button press might have occurred
  shouldStartCounter = true; // Indicate the need to start the debounce counter

  possibleMediumPress = true; // Indicate that a medium button press might have occurred
}








void Gameplay() {
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

  incrementCounter();
  // printGameStats(); //TODO MOVE FROM INSIDE incrementCounter() /Remake?

  gameOver = checkGameOver();
  if (gameOver)
  {
    generateMap();
    stateControl = END_MESSAGE_CODE;
    if (number<255)
      printEndMessage(number);
    else
      printEndMessage(255);
    gameOver = false;
  }
}


void incrementCounter() {
  if (!paused) {
    if (millis() - lastIncrement > delayCount) {
      number++;
      number %= 10000;  
      lastIncrement = millis();
      printGameStats();
    }
  }
  // writeNumber(number);
}
void printGameStats() {
  short sum = computeSum() - 1;
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Remaining: "));
  lcd.setCursor(12, 0);
  lcd.print(sum/100);
  lcd.setCursor(13,0);
  lcd.print((sum/10)%10);
  lcd.setCursor(14,0);
  lcd.print(sum%10);
  
  lcd.setCursor(0, 1);
  lcd.print(F("Time: "));
  lcd.setCursor(12, 1);
  lcd.print(number/100);
  lcd.setCursor(13,1);
  lcd.print((number/10)%10);
  lcd.setCursor(14,1);
  lcd.print(number%10);
}



void generateMap() {
  // Calculate the number of true values to fill between 60% and 85% of the matrix
  int minTrueCount = minFillPercentage * matrixSize * matrixSize;
  int maxTrueCount = maxFillPercentage * matrixSize * matrixSize;
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
      printGameStats();
    }
    
  }

  
}


short computeSum() {
  short sum = 0;
  for (short i=0; i<matrixSize; ++i) 
    for (short j=0; j<matrixSize; ++j) {
      sum += matrix[i][j];
    }
  return sum;
}
bool checkGameOver() {
  short sum = computeSum();
  if (sum > 1)
    return false;
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
  if (xValue < readingMinThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    } else {
      // xPos = 0;
    }
  }
  if (xValue > readingMaxThreshold) {
    if (xPos > 0) {
      xPos--;
    } else {
      // xPos = matrixSize - 1;
    }
  }
  if (yValue > readingMaxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    } else {
      // yPos = 0;
    }
  }
  // Update xPos based on joystick movement (Y-axis)
  if (yValue < readingMinThreshold) {
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















