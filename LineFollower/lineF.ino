#include <QTRSensors.h>

// Motor Pins
const int m11Pin = 7;
const int m12Pin = 6;
const int m21Pin = 5;
const int m22Pin = 4;
const int m1Enable = 11;
const int m2Enable = 10;

// Motor Speed
int m1Speed = 0;
int m2Speed = 0;

// PID Controller Variables
float kp = 8;
float ki = 0.0001;
float kd = 2;
int p = 1;
int i = 0;
int d = 0;
int error = 0;
int lastError = 0;

// Motor Speed constants
const int maxSpeed = 255;
const int minSpeed = -200;
const int baseSpeed = 220;

QTRSensors qtr;
const int sensorCount = 6;
int sensorValues[sensorCount];
int sensors[sensorCount] = { 0, 0, 0, 0, 0, 0 };

bool m1 = false;
bool m2 = false;

// constants used for calibration.
const int maxNumberOfMoves = 10;
const int sensorCalibrationBound = 600;


void setup() {
  // pinMode setup
  pinMode(m11Pin, OUTPUT);
  pinMode(m12Pin, OUTPUT);
  pinMode(m21Pin, OUTPUT);
  pinMode(m22Pin, OUTPUT);
  pinMode(m1Enable, OUTPUT);
  pinMode(m2Enable, OUTPUT);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ A0, A1, A2, A3, A4, A5 }, sensorCount);
  delay(5000); // small delay to allow the user to place the robot on the ground
  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode
  calibrate();
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED

  Serial.begin(9600);
}


void loop() {
  int error = map(qtr.readLineBlack(sensorValues), 0, 5000, -50, 50);
  
  pidControl(error);
  setMotorSpeed(m2Speed, m1Speed);
}


void calibrate() {
  int speed = 50; 
  int state = 1;
  int numberOfMoves = 0;

  while (numberOfMoves < maxNumberOfMoves) {
    qtr.calibrate();
    qtr.read(sensorValues);

    if (state == 1) {
      // Check if the leftmost sensor value is below the calibrated threshold
      if (sensorValues[0] < sensorCalibrationBound) {
        // Adjust motor speeds to turn left.
        setMotorSpeed(-speed, speed);
      } else {
        // If leftmost sensor is not below the threshold, transition to state 2
        state = 2;
      }
    }
    if (state == 2) {
      // Check if the rightmost sensor value is below the calibrated threshold
      if (sensorValues[5] < sensorCalibrationBound) {
        // Adjust motor speeds to turn right
        setMotorSpeed(speed, -speed);
      } else {
        // If rightmost sensor is not below the threshold, transition back to state 1 and increment the number of moves
        state = 1;
        numberOfMoves++;
      }
    }
  }
}


void pidControl(float error) {
  p = error;
  i = i + error;
  d = error - lastError;
  lastError = error;  

  int motorSpeed = kp * p + ki * i + kd * d; 
  
  m1Speed = baseSpeed;
  m2Speed = baseSpeed;
  
  if (error < 0) {
    m1Speed += motorSpeed;
  } else if (error > 0) {
    m2Speed -= motorSpeed;
  }
  
  m1Speed = constrain(m1Speed, minSpeed, maxSpeed);
  m2Speed = constrain(m2Speed, minSpeed, maxSpeed);
}


void setMotorSpeed(int motor1Speed, int motor2Speed) {
  // motor1Speed = -motor1Speed;
  // motor2Speed = -motor2Speed;
  if (motor1Speed == 0) {
    digitalWrite(m11Pin, LOW);
    digitalWrite(m12Pin, LOW);
    analogWrite(m1Enable, motor1Speed);
  } else {
    if (motor1Speed > 0) {
      digitalWrite(m11Pin, HIGH);
      digitalWrite(m12Pin, LOW);
      analogWrite(m1Enable, motor1Speed);
    }
    if (motor1Speed < 0) {
      digitalWrite(m11Pin, LOW);
      digitalWrite(m12Pin, HIGH);
      analogWrite(m1Enable, -motor1Speed);
    }
  }
  if (motor2Speed == 0) {
    digitalWrite(m21Pin, LOW);
    digitalWrite(m22Pin, LOW);
    analogWrite(m2Enable, motor2Speed);
  } else {
    if (motor2Speed > 0) {
      digitalWrite(m21Pin, HIGH);
      digitalWrite(m22Pin, LOW);
      analogWrite(m2Enable, motor2Speed);
    }
    if (motor2Speed < 0) {
      digitalWrite(m21Pin, LOW);
      digitalWrite(m22Pin, HIGH);
      analogWrite(m2Enable, -motor2Speed);
    }
  }
}
