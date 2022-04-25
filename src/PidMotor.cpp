
#include "PidMotor.h"

PidMotor::PidMotor(uint8_t dirPin1, uint8_t dirPin2, uint8_t pwmPin, uint8_t encPinA, uint8_t encPinB) : 
  filter(15, 1e3, true)
{
    _dirPin1 = dirPin1;
    _dirPin2 = dirPin2;
    _pwmPin = pwmPin;
    _encPinA = encPinA;
    _encPinB = encPinB;

    pinMode(_dirPin1,OUTPUT);
    pinMode(_dirPin2,OUTPUT);
    pinMode(_pwmPin,OUTPUT);
    pinMode(_encPinA,INPUT);
    pinMode(_encPinB,INPUT);

    // Velocity
    targetVel = 0;
    curVel = 0;
    pos = 0;

    // Memory
    prevErrorIntegral = 0;
    prevPos = 0;
    prevTime = 0;
    
    // PID Constants
    kp = 0;
    ki = 0;
}

void PidMotor::setInterrupt(void (*interrupt)(void)) {
    attachInterrupt(digitalPinToInterrupt(_encPinA), interrupt, RISING);
}

void PidMotor::setMotorSpeed(float velocity) {
    targetVel = velocity;
    prevErrorIntegral = 0;
}

void PidMotor::update(void) {
  // Update time
  long currTime = micros();
  float deltaTime = ((float) (currTime - prevTime))/( 1.0e6 );
  prevTime = currTime;

  //Update Velocity
  int measuredPos = this->getPos();
  float currentVel = (measuredPos - prevPos) / deltaTime / 90;
  prevPos = measuredPos;

  // Use Filter
  float currentVelFilt = filter.filt(currentVel);

  // Compute output of PID
  float error = targetVel - currentVelFilt;
  float currErrorIntegral = prevErrorIntegral + error * deltaTime;
  float u = (kp * error) + (ki * currErrorIntegral);

  // Clamping
  if (!(error > 0 && u > 255) && !(error < 0 && u < -255)) {
    prevErrorIntegral = currErrorIntegral;
  }

  // Translate to Motor Command
  int dir = 1;
  if (u < 0) {
    dir = -1;
  }
  int pwmVal = (int) fabs(u);
  if (pwmVal > 255) {
    pwmVal = 255;
  }

  // Run the Motor
  // this->runMotor(dir, pwmVal);
  curVel = currentVelFilt;
}

void PidMotor::runMotor(int dir, int pwmVal){
  analogWrite(_pwmPin,pwmVal); // Motor speed
  if(dir == 1){ 
    // Turn one way
    digitalWrite(_dirPin1,HIGH);
    digitalWrite(_dirPin2,LOW);
  }
  else if(dir == -1){
    // Turn the other way
    digitalWrite(_dirPin1,LOW);
    digitalWrite(_dirPin2,HIGH);
  }
  else{
    // Or dont turn
    digitalWrite(_dirPin1,LOW);
    digitalWrite(_dirPin2,LOW);
  }
}

int PidMotor::getPos(void) {
  return pos;
}

float PidMotor::getVelocity(void) {
  return curVel;
}

void PidMotor::pulseEncoder() {
  int b = digitalRead(_encPinB);
  int increment = 0;
  if(b > 0){
    increment = 1;
  } else{
    increment = -1;
  }
  pos = pos + increment;
}