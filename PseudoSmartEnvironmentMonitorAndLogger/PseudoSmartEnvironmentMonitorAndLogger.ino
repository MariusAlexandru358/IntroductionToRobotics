#include <EEPROM.h>


const byte redLedPin=11;
const byte greenLedPin=10;
const byte blueLedPin=9;

const byte trigPin = 5;
const byte echoPin = 6;
long duration = 0;
int distance = 0;

const byte photocellPin = A0;     // the cell and 10K pulldown are connected to a0
int photocellValue;     // the analog reading from the sensor divider

byte n = 10;
byte distanceReadings[10];
unsigned short int lightReadings[10];
byte distanceIndex = 0;
byte lightIndex = 0;
byte distanceIndexEEPROM = 10;
byte lightIndexEEPROM = 20;
unsigned short int currentValueEEPROM = 0;


byte menuControl = 0;

char input;
int settingsInput = 0;
unsigned long lastReadingTime = 0;
const short int samplingRateMillisecondsFactor = 1000;
byte samplingRate = 10;
byte samplingRateIntervalLimits[2] = {1, 10};
short int ultrasonicThreshold = 50;
bool ultrasonicThresholdMode = true; // True if the threshold is a min value (alert if an object is too close). False if the threshold is a max value (alert if an object is too far).
short int LDRThreshold = 250;
bool LDRThresholdMode = true; // True if the threshold is a min value (alert if it's too dark). False if the threshold is a max value (alert if it's too bright).

//TODO 2

bool printReadings = false;

byte inputValuesRGB = 0;
byte inputRedVal = 0;
byte inputGreenVal = 0;
byte inputBlueVal = 0;
bool formatRGB = true;

byte defaultGoodRedVal = 0;
byte defaultGoodGreenVal = 100;
byte defaultGoodBlueVal = 0;

byte defaultAlertRedVal = 100;
byte defaultAlertGreenVal = 0;
byte defaultAlertBlueVal = 0;

byte customGoodRedVal = 0;
byte customGoodGreenVal = 100;
byte customGoodBlueVal = 0;

byte customAlertRedVal = 100;
byte customAlertGreenVal = 0;
byte customAlertBlueVal = 0;

bool automatic = true;
bool alert = false;


void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input

  pinMode(photocellPin, INPUT);

  Serial.begin(9600);
  printMainMenu();
}


void loop() {
  switch (menuControl) {
  case 0:
    MainMenu();
    break;
  case 10:
    SensorSettings();
    break;
  case 30:
    SystemStatus();
    break;
  case 40:
    RBGControl();
    break;
  default:
    MainMenu();
    break;
  }

  handleSensors();
  RGBLED();
}


void MainMenu() {
  if (Serial.available() > 0) {

    input = (char) Serial.read();
    switch (input) {
    case '1':
      menuControl = 10;
      printSensorSettingsMenu();
      SensorSettings();
      break;
    case '2':
      // menuControl = 20;
      printResetDataMenu();
      ResetData();
      break;
    case '3':
      menuControl = 30;
      printSystemStatusMenu();
      SystemStatus();
      break;
    case '4':
      menuControl = 40;
      printRBGControlMenu();
      RBGControl();
      break;
    case '?':
      Serial.println(F("Help already? Only the number of the option is expected."));
      printMainMenu();
      break;
    default:
      Serial.println("Invalid option. Please try again.");
      printMainMenu();
      break;
    }

  }
}


void SensorSettings() {
  if (Serial.available() > 0) {
    input = (char) Serial.read();
    switch (input) {
    case '1':
      // menuControl = 11;
      Serial.println(F("\t\tSampling rate setting:"));
      Serial.println(F("\t\tEnter a value between 1 and 10 (seconds):"));
      while (!Serial.available()) {
      }
      if (Serial.available() > 0) {
        settingsInput = Serial.parseInt();
        if (settingsInput <= samplingRateIntervalLimits[1] && settingsInput >= samplingRateIntervalLimits[0])
          samplingRate = settingsInput;
        else
          Serial.println(F("\t\tInvalid value. Please try again."));
      }
      printSensorSettingsMenu();
      break;

    case '2':
      // menuControl = 12;
      Serial.println(F("\t\tUltrasonic alert threshold setting:"));
      Serial.println(F("\t\tEnter a value for the threshold: A positive value for a min limit (alert if an object is too close), or a negative value for a max limit (alert if an object is too far)"));
      while (!Serial.available()) {
      }
      if (Serial.available() > 0) {
        settingsInput = Serial.parseInt();
        if (settingsInput > 0 && settingsInput < 1000) {
          ultrasonicThresholdMode = true;
          ultrasonicThreshold = settingsInput;
        }
        else if (settingsInput < 0 && settingsInput > -1000) {
          ultrasonicThresholdMode = false;
          ultrasonicThreshold = (-1) * settingsInput;
        }
        else {
          Serial.println(F("\t\tNot a valid threshold. Please try again. Expected value between (-1000, 1000)"));
        }
      }
      break;

    case '3':
      // menuControl = 13;
      Serial.println(F("\t\tLDR alert threshold setting:"));
      Serial.println(F("\t\tEnter a value for the threshold: A positive value for a min limit (alert if it's too dark), or a negative value for a max limit (alert if it's too bright)"));
      while (!Serial.available()) {
      }
      if (Serial.available() > 0) {
        settingsInput = Serial.parseInt();
        if (settingsInput > 0 && settingsInput < 1000) {
          LDRThresholdMode = true;
          LDRThreshold = settingsInput;
        }
        else if (settingsInput < 0 && settingsInput > -1000) {
          LDRThresholdMode = false;
          LDRThreshold = (-1) * settingsInput;
        }
        else {
          Serial.println(F("\t\tNot a valid threshold. Please try again. Expected value between (-1000, 1000)"));
        }
      }
      break;

    case '4':
      menuControl = 0;
      printMainMenu();
      break;

    case '?':
      Serial.println(F("\t(1) Help? Only the number of the option is expected."));
      printSensorSettingsMenu();
      break;
    default:
      Serial.println(F("\tInvalid option. Please try again."));
      printSensorSettingsMenu();
      break;
    }
  }
}


void ResetData() {
  
  if (Serial.available() > 0) {
    input = (char) Serial.read();
    switch (input) {
    case '1':
      for (int i=distanceIndexEEPROM; i<distanceIndexEEPROM+n; i++) {
        EEPROM.update(i,0);
      }
      for (int i=lightIndexEEPROM; i<lightIndexEEPROM+n*sizeof(currentValueEEPROM); i++) {
        EEPROM.update(i,0);
        Serial.print(", ");
      }
      break;

    case '2':
      printMainMenu();
      menuControl = 0;
      break;
    
    case '?':
      Serial.println(F("\t(2) Help? Only the number of the option is expected."));
      printResetDataMenu();
      break;
    default:
      Serial.println(F("\tInvalid option. Please try again."));
      printResetDataMenu();
      break;
    }
  }
}


void SystemStatus() {
  if (Serial.available() > 0) {
    input = (char) Serial.read();
    switch (input) {
    case '1':
      // menuControl = 31;
      Serial.println(F("\t\tSensor readings below. Enter 'c' to stop and return to the menu."));
      printReadings = true;
      displayReadings();
      break;

    case '2':
      // menuControl = 30;
      Serial.println(F("\t\tSensor settings below:"));
      Serial.print(F("\t\tSampling rate: every "));
      Serial.print(samplingRate);
      Serial.print(F(" seconds\n"));
      
      Serial.print(F("\t\tUltrasonic threshold: "));
      Serial.print(ultrasonicThreshold);
      if (ultrasonicThresholdMode) 
        Serial.print(F(" (min value, alert if lower)\n"));
      else
        Serial.print(F(" (max value, alert if higher)\n"));

      Serial.print(F("\t\tLDR threshold: "));
      Serial.print(LDRThreshold);
      if (LDRThresholdMode) 
        Serial.print(F(" (min value, alert if lower)\n"));
      else
        Serial.print(F(" (max value, alert if higher)\n"));

      break;

    case '3':
      // menuControl = 30;
      Serial.println(F("\t\tLast logged data:"));
      Serial.print(F("\t\tDistance: "));
      for (int i=distanceIndexEEPROM; i<distanceIndexEEPROM+n; i++) {
        currentValueEEPROM = EEPROM.read(i);
        Serial.print(currentValueEEPROM);
        Serial.print(", ");
      }
      Serial.print(F("\n\t\tLight: "));
      for (int i=lightIndexEEPROM; i<lightIndexEEPROM+n*sizeof(currentValueEEPROM); i+=sizeof(currentValueEEPROM)) {
        EEPROM.get(i,currentValueEEPROM);
        Serial.print(currentValueEEPROM);
        Serial.print(", ");
      }
      Serial.print("\n");
      break;
    
    case '4':
      menuControl = 0;
      printMainMenu();
      //TODO
      break;
    
    case '?':
      Serial.println(F("\t(3) Help? Only the number of the option is expected."));
      printSystemStatusMenu();
      break;
    case 'c':
      printReadings = false;
      printSystemStatusMenu();
      break;
    default:
      Serial.println(F("\tInvalid option. Please try again."));
      printSystemStatusMenu();
      break;
    }
  }
}


void RBGControl() {
  
  if (Serial.available() > 0) {
    input = (char) Serial.read();
    switch (input) {
    case '1':
      // menuControl = 41;
      Serial.println(F("\t\t1. Set RGB color for normal parameters: "));
      Serial.println(F("\t\t2. Set RGB color for alert: "));
      while (!Serial.available()) {
      }
      if(Serial.available() > 0) {
        Serial.println("RGB GOOD SETTINGS");
        
        input = (char) Serial.read();
        switch (input) {
        case '1':
          formatRGB = true;

          Serial.println("\t\tRed: ");
          while (!Serial.available()) {
          }
          if (Serial.available() > 0) {
            inputValuesRGB = Serial.parseInt();
            if (inputValuesRGB >= 0 && inputValuesRGB < 256)
              inputRedVal = inputValuesRGB;
            else
              formatRGB = false;

            Serial.println("\t\tGreen: ");
            while (!Serial.available()) {
            }
            if(Serial.available() > 0 && formatRGB) {
              inputValuesRGB = Serial.parseInt();
              if (inputValuesRGB >= 0 && inputValuesRGB < 256)
                inputGreenVal = inputValuesRGB;
              else
                formatRGB = false;
              Serial.println("\t\tBlue: ");
              while (!Serial.available()) {
              }
              if (Serial.available() > 0 && formatRGB) {
                inputValuesRGB = Serial.parseInt();
                if (inputValuesRGB >= 0 && inputValuesRGB < 256)
                  inputBlueVal = inputValuesRGB;
                else
                  formatRGB = false;
              }
            }
          }
          if (formatRGB) {
            customGoodRedVal = inputRedVal;
            customGoodGreenVal = inputGreenVal;
            customGoodBlueVal = inputBlueVal;
          }
          else{
              Serial.println(F("Invalid value. Please try again"));
          }
          break;

        case '2':
          formatRGB = true;
          Serial.println("\t\tRed: ");
          while (!Serial.available()) {
          }
          if (Serial.available() > 0) {
            inputValuesRGB = Serial.parseInt();
            if (inputValuesRGB >= 0 && inputValuesRGB < 256)
              inputRedVal = inputValuesRGB;
            else
              formatRGB = false;

            Serial.println("\t\tGreen: ");
            while (!Serial.available()) {
            }  
            if(Serial.available() > 0 && formatRGB) {
              inputValuesRGB = Serial.parseInt();
              if (inputValuesRGB >= 0 && inputValuesRGB < 256)
                inputGreenVal = inputValuesRGB;
              else
                formatRGB = false;

              Serial.println("\t\tBlue: ");
              while (!Serial.available()) {
              }
              if (Serial.available() > 0 && formatRGB) {
                inputValuesRGB = Serial.parseInt();
                if (inputValuesRGB >= 0 && inputValuesRGB < 256)
                  inputBlueVal = inputValuesRGB;
                else
                  formatRGB = false;
              }
            }
          }
          if (formatRGB) {
            customAlertRedVal = inputRedVal;
            customAlertGreenVal = inputGreenVal;
            customAlertBlueVal = inputBlueVal;
          }
          else{
            Serial.println(F("Invalid value. Please try again"));
          }
          break;

        case '?':
          Serial.println(F("\tCUSTOM RGB Help? Only the number of the option is expected."));
          break;
        default:
          Serial.println(F("\tInvalid option. Please try again."));
          break;
        }
      }
      break;

    case '2':
      // menuControl = 42;
      Serial.println("\t\tAutomatic mode:");
      Serial.println("\t\t1. ON");
      Serial.println("\t\t2. OFF");
      while (!Serial.available()) {
      }
      if (Serial.available() > 0) {
        input = (char) Serial.read();
        switch (input) {
        case '1':
          automatic = true;
          break;
        case '2':
          automatic = false;
          break;
        case '?':
          Serial.println(F("\tHelp? Just select one of possible options. Only the number of the option is expected."));
          break;
        default:
          Serial.println(F("\tInvalid option. Please try again."));
          break;    
        }
      }
      break;

    case '3':
      menuControl = 0;
      printMainMenu();
      
      break;

    case '?':
      Serial.println(F("\tHelp? Just select one of possible options. Only the number of the option is expected."));
      printRBGControlMenu();
      break;
    default:
      Serial.println(F("\tInvalid option. Please try again."));
      printRBGControlMenu();
      break;
    }

    printRBGControlMenu();
  }
}



void displayReadings() {
  //TODO
  input = Serial.read();
  if (input == 'c') {
    printReadings = false;
  }
  if (printReadings) {
    Serial.print("\t\tDistance: ");
    Serial.print(distance);
    Serial.print("  ;  Light: ");
    Serial.print(photocellValue);
    Serial.print("\n");
  }
}


void handleSensors() {
  if (millis() - lastReadingTime > samplingRate * samplingRateMillisecondsFactor) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    photocellValue= analogRead(photocellPin);  

    lastReadingTime = millis();
  
    distanceReadings[distanceIndex] = distance;
    lightReadings[lightIndex] = photocellValue;
    distanceIndex++;
    lightIndex++;
    if (distanceIndex >= n){
      for (int i=distanceIndexEEPROM; i<distanceIndexEEPROM+n; i++) {
        distanceIndex--;
        EEPROM.update(i,distanceReadings[distanceIndex]);
      }
    }
    if (lightIndex >= n) {
      for (int i=lightIndexEEPROM; i<lightIndexEEPROM+n*sizeof(currentValueEEPROM); i+=sizeof(currentValueEEPROM)) {
        lightIndex--;
        EEPROM.put(i,lightReadings[lightIndex]);
      }
    }

    if (ultrasonicThresholdMode) {
      if (distance < ultrasonicThreshold) {
        alert = true;
      }
    }
    else {
      if (distance > ultrasonicThreshold) {
        alert = true;
      }
    }

    if (LDRThresholdMode) {
      if (photocellValue < LDRThreshold) {
        alert = true;
      }
    }
    else {
      if (photocellValue > LDRThreshold) {
        alert = true;
      }
    }

    if ((ultrasonicThresholdMode && distance > ultrasonicThreshold) || (!ultrasonicThresholdMode && distance < ultrasonicThreshold)){
      if ((LDRThresholdMode && photocellValue > LDRThreshold) || (!LDRThresholdMode && photocellValue < LDRThreshold)){
       alert = false;
      }
    }


    if (printReadings) {
      displayReadings();
    }
  }
}


void RGBLED() {
  if (automatic) {
    if (alert) {
      analogWrite(redLedPin, defaultAlertRedVal);
      analogWrite(greenLedPin, defaultAlertGreenVal);
      analogWrite(blueLedPin, defaultAlertBlueVal);
    } 
    else {
      analogWrite(redLedPin, defaultGoodRedVal);
      analogWrite(greenLedPin, defaultGoodGreenVal);
      analogWrite(blueLedPin, defaultGoodBlueVal);
    }
  }
  else{
    if (alert) {
      analogWrite(redLedPin, customAlertRedVal);
      analogWrite(greenLedPin, customAlertGreenVal);
      analogWrite(blueLedPin, customAlertBlueVal);
    } 
    else {
      analogWrite(redLedPin, customGoodRedVal);
      analogWrite(greenLedPin, customGoodGreenVal);
      analogWrite(blueLedPin, customGoodBlueVal);
    }
  }
}



void printMainMenu() {
  Serial.println("\nMenu interface:");
  Serial.println("Select an option:");
  Serial.println("1. Sensor settings");
  Serial.println("2. Reset logger data");
  Serial.println("3. System status");
  Serial.println("4. RGB LED control");
}

void printSensorSettingsMenu() {
  Serial.println("\tSensor settings:");
  Serial.println("\tSelect an option:");
  Serial.println("\t1. Sensor sampling interval");
  Serial.println("\t2. Ultrasonic alert threshold");
  Serial.println("\t3. LDR alert threshold");
  Serial.println("\t4. Back");
}

void printResetDataMenu() {
  Serial.println("\tReset logger data:");
  Serial.println("\tAre you sure?");
  Serial.println("\t1. YES");
  Serial.println("\t2. NO");
}

void printSystemStatusMenu() {
  Serial.println("\tSystem status:");
  Serial.println("\tSelect an option:");
  Serial.println("\t1. Current sensor readings");
  Serial.println("\t2. Current sensor settings");
  Serial.println("\t3. Display logged data");
  Serial.println("\t4. Back");
}

void printRBGControlMenu() {
  Serial.println("\tRGB LED control options:");
  Serial.println("\tSelect an option:");
  Serial.println("\t1. Manual color control");
  Serial.println("\t2. LED: Toggle automatic ON/OFF");
  Serial.println("\t3. Back");
}


