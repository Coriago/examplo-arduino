
#include "PidMotor.h"
#include "LowPass.h"

PidMotor::PidMotor(uint8_t dirPin1, uint8_t dirPin2, uint8_t pwmPin, uint8_t encPinA, uint8_t encPinB)
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

    pos = 0;
    prevPos = 0;
    prevTime = 0;

    velocityFilt = 0;
    velocityPrev = 0;
}

void PidMotor::setInterrupt(void (*interrupt)(void)) {
    attachInterrupt(digitalPinToInterrupt(_encPinA), interrupt, RISING);
}

void PidMotor::setMotorSpeed(float velocity) {
    // Something
}

float PidMotor::update(void) {
  // Update time
  long currTime = micros();
  float deltaTime = ((float) (currTime - prevTime))/( 1.0e6 );
  prevTime = currTime;

  //Update Velocity
  int measuredPos = this->getPos();
  float currentVel = (measuredPos - prevPos) / deltaTime / 90;
  prevPos = measuredPos;

  // velocityFilt = 0.854*velocityFilt + 0.0728*currentVel + 0.0728*velocityPrev;
  // velocityPrev = currentVel;

  // Serial.print(velocityFilt);
  // Serial.print(",");

  return currentVel;
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