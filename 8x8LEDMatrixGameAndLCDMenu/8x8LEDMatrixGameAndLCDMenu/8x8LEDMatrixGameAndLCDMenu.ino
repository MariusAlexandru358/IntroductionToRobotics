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
const byte ledPin = 3; // Pin for the information LED

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // LedControl object to manage the LED matrix


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
byte brightnessSettingOption = 9;
bool cursor = false;








void setup() {
  pinMode(lcdIntensityPin, OUTPUT);
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

  EEPROM.update(EEPROM_MATRIX_BRIGHTNESS_ADDRESS, 2);
  byte lcIntensity = EEPROM.read(EEPROM_MATRIX_BRIGHTNESS_ADDRESS);
  lc.setIntensity(0, lcIntensity);  // sets matrix brightness (0~15 possible values)

  EEPROM.update(EEPROM_LCD_BRIGHTNESS_ADDRESS, 9);
  byte lcdIntensityVol = EEPROM.read(EEPROM_LCD_BRIGHTNESS_ADDRESS);
  byte lcdIntensityVal = map(lcdIntensityVol, 0, 9, 55, 255);
  analogWrite(lcdIntensityPin, lcdIntensityVal);

  Serial.begin(9600);
  // printMainMenu(MENU_OPTION_1);

  // stateControl = MAIN_MENU_CODE;
  // printMainMenu(MENU_OPTION_1);
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
  lcd.setCursor(8,1);
  lcd.cursor(); //TOTEST is an on/off cycle needed to blink?
}
void MatrixBrightnessMenu() {
  unsigned long currentMillis;
  input = detectInput();
  switch (input) {
  case INPUT_UP:
    MatrixBrightnessAction(MENU_OPTION_BACK);
    break;
  case INPUT_DOWN:
    break;
  case INPUT_LEFT:
    menuPosition--;
    if (menuPosition < 0)
      menuPosition = 0;
    // byte val = map(menuPosition, 0, 9, 1, 10);
    lc.setIntensity(0, menuPosition+1);  // sets matrix brightness (0~15 possible values)
    lcd.setCursor(2+menuPosition,1);
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
    break;
  case INPUT_RIGHT:
    menuPosition++;
    if (menuPosition > 9)
      menuPosition = 9;
    // byte val = map(menuPosition, 0, 9, 1, 10);
    lc.setIntensity(0, menuPosition+1);  // sets matrix brightness (0~15 possible values)
    lcd.setCursor(2+menuPosition,1);
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
    break;
  case INPUT_CLICK:
    lcd.noCursor();
    cursor = false;
    MatrixBrightnessAction(menuPosition+1);
    break;
  default:
    break;  
  }
}
void MatrixBrightnessAction(short selectedOption) {
  switch (selectedOption) {
  case MENU_OPTION_BACK:
    stateControl = SETTINGS_CODE;
    menuPosition = MENU_OPTION_1; //reset the menuPosition for the next time we enter a menu or submenu
    printSettingsMenu(MENU_OPTION_1);
    SettingsMenu();
    break;
  default:
    EEPROM.update(EEPROM_MATRIX_BRIGHTNESS_ADDRESS, selectedOption);
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



void printEndMessage(short score=999) {
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
  if (input == MENU_OPTION_BACK) {
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
}



void Gameplay() {

}












