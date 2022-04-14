#include "src/PidMotor.h"
#include "src/LowPass.h"

const int MOTOR_COUNT = 4;
int input = 0;
int direction = 0;
float cutoff = 0;

struct step {
  int input;
  float time;
};
const int Nsteps = 7;
step steps[Nsteps] = {
  {255, 1.6},
  {-255, 2.1},
  {0, 2.8},
  {150, 3.7},
  {200, 4.2},
  {-70, 5.6},
  {0, 6}
};


PidMotor motors[] ={
  PidMotor(45,44,10,20,13),
  PidMotor(47,46,5,3,23),
  PidMotor(48,49,6,2,22),
  PidMotor(8,9,11,21,12),
};

// Filter instance
LowPass lp(10,1e4,true);

void setup() {
  Serial.begin(115200);
  motors[0].setInterrupt(encoder_interrupt<0>);
  motors[1].setInterrupt(encoder_interrupt<1>);
  motors[2].setInterrupt(encoder_interrupt<2>);
  motors[3].setInterrupt(encoder_interrupt<3>);

  // for (PidMotor x : motors)
  //   x.runMotor(1,160);
}

void loop() {
  
  float currTime = micros()/( 1.0e6 );

  for (int i = 0; i < Nsteps; i++) {
    if ( currTime < steps[i].time) {
      input = steps[i].input;
      break;
    }
  }

  // Direction
  if (input < 0) {
    input = input * -1;
    direction = -1;
  } else {
    direction = 1;
  }

  for (PidMotor x : motors)
    x.runMotor(direction, input);

  if (currTime > cutoff) {
    for (PidMotor x : motors)
      x.runMotor(0, 0);
    exit(0);
  }

  float xn = motors[0].update();
  float output = lp.filt(xn);
  
  Serial.print(currTime);
  Serial.print(",");
  Serial.print(input);
  Serial.print(",");
  Serial.println(output);
  delay(1);
}

template<int i>
void encoder_interrupt(void) {
  motors[i].pulseEncoder();
}

