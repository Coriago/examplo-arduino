#ifndef PidMotor_H
#define PidMotor_H

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "LowPass.h"


class PidMotor {
public:
    PidMotor(uint8_t dirPin1, uint8_t dirPin2, uint8_t pwmPin, uint8_t encPinA, uint8_t encPinB);
    void setInterrupt(void (*interrupt)(void));
    void setMotorSpeed(float velocity);
    void runMotor(int dir, int pwmVal);
    void update(void);
    void pulseEncoder(void);
    int getPos(void);
    float getVelocity(void);
    int getPwm(void);

    const int ROTATION_COUNT = 90;

private:
    uint8_t _dirPin1, _dirPin2, _pwmPin, _encPinA, _encPinB;
    int pos, prevPos, currentPwm;
    long prevTime;
    float targetVel, curVel, prevErrorIntegral, kp, ki;
    LowPass filter;
};

#endif
