
const int floorsTotal = 3; // total number of floors
const int floorButtonPins[] = {4,3,2}; // array of pin numbers for the floor buttons
const int floorLedPins[] = {12,11,10}; // array of pin numbers for the floor LEDs
const int musicBuzzerPin = 7; // pin number for the buzzer
const int musicLedPin = 13; // pin number for the operational state LED

int currentFloor = 0; 
int destinationFloor = 0; 
bool action = false; // flag marking if the elevator is executing a movement or waiting for an input
bool doors = true; // flag marking if the doors are opened or closed
bool passingBy = false; // flag marking the passing by another floor
int distance = 0;
int direction = 1;
int i;

unsigned long firstActionTime = 0; // time of the first action (time of the confirmed button push)
unsigned long currentActionTime = 0; // current time
unsigned long lastMusicLedToggleTime = 0; // time of the last toggle of the operational state LED
unsigned long lastTravelTime = 0; // time for travelling between floors
unsigned long lastMusicSoundTime = 0; // time for the elevator music

const int musicInterval = 200; // reference interval for the music (milliseconds) 
const int doorsInterval = 500; // reference interval for closing the doors (milliseconds) 
const int passingByInterval = 500; // reference interval for signaling the passing by another floor (milliseconds) 
const int oneFloorTravelInterval = 3000; // time it takes to travel between 2 adjacent floors (milliseconds) 

byte musicLedState = HIGH; // state of the operational state LED
byte floorButtonStates[] = {HIGH, HIGH, HIGH}; // array of states for the floor buttons
byte lastFloorButtonStates[] = {HIGH, HIGH, HIGH}; // array of last states of the floor buttons, used for debouncing
byte floorLedStates[] = {HIGH, LOW, LOW}; // array of states for the floor LEDs

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  for (i=0; i<floorsTotal; i++) {
    pinMode(floorButtonPins[i], INPUT_PULLUP);
    pinMode(floorLedPins[i], OUTPUT);
  }
  pinMode(musicBuzzerPin, OUTPUT);
  pinMode(musicLedPin, OUTPUT);
  
  digitalWrite(musicLedPin, musicLedState);
  digitalWrite(floorLedPins[0], HIGH);
}

void loop() {
  for (i=0; i<floorsTotal && action==false; i++) {
    int reading = digitalRead(floorButtonPins[i]);
    if (reading != lastFloorButtonStates[i]) {
      lastDebounceTime = millis();
    }

    if ((millis()-lastDebounceTime) > debounceDelay) {
      if (reading != floorButtonStates[0]) {
        Serial.println(5);
        floorButtonStates[0] = reading;
        if (floorButtonStates[0] == LOW) {
          action = true;                              
          destinationFloor = i;                       
          distance = destinationFloor - currentFloor;
          if (distance < 0) {
            direction = -1;
          }
          firstActionTime = millis();
          tone(musicBuzzerPin, 260, musicInterval*2);
          lastMusicLedToggleTime = firstActionTime;
          lastMusicSoundTime = firstActionTime;
          lastTravelTime = firstActionTime;
          doors = true;
        }
      }
    }
    lastFloorButtonStates[i] = reading;
  }
  
  if (action) {
    currentActionTime = millis(); 
    if (distance != 0) {
      
      // DEFAULT ELEVATOR MUSIC
      if (!doors &&  currentActionTime - lastMusicSoundTime > musicInterval*2  &&  currentActionTime - lastMusicSoundTime < musicInterval*4) {
        tone(musicBuzzerPin, 350);
      }
      if (!doors &&  currentActionTime - lastMusicSoundTime > musicInterval*4  &&  currentActionTime - lastMusicSoundTime < musicInterval*6) {
        noTone(musicBuzzerPin);
      }
      if (!doors &&  currentActionTime - lastMusicSoundTime > musicInterval*6  &&  currentActionTime - lastMusicSoundTime > musicInterval*8) {
        tone(musicBuzzerPin, 440);
      }
      if (!doors &&  currentActionTime - lastMusicSoundTime > musicInterval*8  &&  currentActionTime - lastMusicSoundTime > musicInterval*10) {
        noTone(musicBuzzerPin);
        lastMusicSoundTime = currentActionTime;
      }

      if (currentActionTime - lastMusicLedToggleTime > musicInterval) {
        lastMusicLedToggleTime = currentActionTime;
        musicLedState = !musicLedState;
        digitalWrite(musicLedPin, musicLedState);
      }

      if (doors  &&  currentActionTime - lastTravelTime > doorsInterval) {
        doors = false;
        lastMusicSoundTime = currentActionTime; // We have closed the doors, the music can start playing now. We set the music timer
        lastTravelTime = currentActionTime;
        floorLedStates[currentFloor] = LOW;
        digitalWrite(floorLedPins[currentFloor], floorLedStates[currentFloor]);
      }

      if (passingBy  &&  currentActionTime - lastTravelTime > passingByInterval) { // has passed by
        passingBy = false;
        floorLedStates[destinationFloor-distance] = LOW;
        digitalWrite(floorLedPins[destinationFloor-distance], floorLedStates[destinationFloor-distance]);
      }

      if (currentActionTime - lastTravelTime > oneFloorTravelInterval) { // passing by next floor
        distance = distance - direction;
        lastTravelTime = currentActionTime;
        passingBy = true;
        floorLedStates[destinationFloor-distance] = HIGH;
        digitalWrite(floorLedPins[destinationFloor-distance], floorLedStates[destinationFloor-distance]);

        if (distance == 0) {
          tone(musicBuzzerPin, 1200, musicInterval*1.5);
          tone(musicBuzzerPin, 1800, musicInterval);
        }
      }

    }
    else { // called the same floor or has arrived at destinationFloor
      action = false;
      doors = true;
      passingBy = false;

      musicLedState = HIGH; 
      digitalWrite(musicLedPin, musicLedState);

      currentFloor = destinationFloor;
      distance = 0;
      direction = 1;
    }
  }

}
