const int maxAnalogVal=1023;
const int maxDigitalVal=255;

const int redLedPin=11;
const int greenLedPin=10;
const int blueLedPin=9;

const int redInputPin = A1;
const int greenInputPin = A2;
const int blueInputPin = A0;

int redInputVal=0;
int greenInputVal=0;
int blueInputVal=0;

int redLedVal=0;
int greenLedVal=0;
int blueLedVal=0;

void setup() {
  // 
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  // 
  pinMode(redInputPin, INPUT);
  pinMode(greenInputPin, INPUT);
  pinMode(blueInputPin, INPUT);
  // Serial.begin(9600);
}

void loop() {
  // 
  redInputVal=analogRead(redInputPin);
  greenInputVal=analogRead(greenInputPin);
  blueInputVal=analogRead(blueInputPin);
  
  redLedVal = map(redInputVal, 0, maxAnalogVal, 0, maxDigitalVal); 
  greenLedVal = map(greenInputVal, 0, maxAnalogVal, 0, maxDigitalVal);   
  blueLedVal = map(blueInputVal, 0, maxAnalogVal, 0, maxDigitalVal); 
  
  analogWrite(redLedPin, redLedVal);
  analogWrite(greenLedPin, greenLedVal);
  analogWrite(blueLedPin, blueLedVal);
  
}
