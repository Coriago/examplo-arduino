#include "src/PidMotor.h"

const int ROTATION_COUNT = 90;
const int MOTOR_COUNT = 4;

PidMotor motors[] ={
  PidMotor(45,44,10,20,13),
  PidMotor(47,46,5,3,23),
  PidMotor(48,49,6,2,22),
  PidMotor(8,9,11,21,12),
};


void setup() {
  Serial.begin(115200);
  motors[0].setInterrupt(encoder_interrupt<0>);
  motors[1].setInterrupt(encoder_interrupt<1>);
  motors[2].setInterrupt(encoder_interrupt<2>);
  motors[3].setInterrupt(encoder_interrupt<3>);

  // for (PidMotor x : motors)
  //   x.runMotor(1,200);
}

void loop() {
  
  for (int i = 0; i < MOTOR_COUNT; i++) {
    Serial.print(i);
    Serial.print(":");
    motors[i].update();
    Serial.print(",");
  }
  Serial.println();

  delay(10);
        
}

template<int i>
void encoder_interrupt(void) {
  motors[i].pulseEncoder();
}

