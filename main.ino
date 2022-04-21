#include "src/PidMotor.h"
#include "src/SerialComms.h"

const byte MOTOR_COUNT = 4;
SerialComms comms;

PidMotor motors[] ={
  PidMotor(45,44,10,20,13),
  PidMotor(47,46,5,3,23),
  PidMotor(48,49,6,2,22),
  PidMotor(8,9,11,21,12),
};

template<int i>
void encoder_interrupt(void) {
  motors[i].pulseEncoder();
}

void setup() {
  motors[0].setInterrupt(encoder_interrupt<0>);
  motors[1].setInterrupt(encoder_interrupt<1>);
  motors[2].setInterrupt(encoder_interrupt<2>);
  motors[3].setInterrupt(encoder_interrupt<3>);

  Serial.begin(115200);
  while(! Serial) {}
  Serial.flush();
  Serial.println("<Arduino is ready>");
}

void runMotors() {
  int efforts[MOTOR_COUNT];
  comms.parseMotorEffort(efforts, MOTOR_COUNT);
  int direction;
  int pwmVal;
  for (byte i = 0; i < MOTOR_COUNT; i++) {
    if (efforts[i] < 0) {
      direction = -1;
      pwmVal = efforts[i] * -1;
    } else {
      direction = 1;
      pwmVal = efforts[i];
    }
    motors[i].runMotor(direction, pwmVal);
  }
  comms.sendRecieved();
}

void stopMotors() {
  for (byte i = 0; i < MOTOR_COUNT; i++) {
    motors[i].runMotor(0, 0);
  }
  comms.sendRecieved();
}

void sendSensorData() {
  float vels[MOTOR_COUNT];
  for (byte i = 0; i < MOTOR_COUNT; i++) {
    vels[i] = motors[i].update();
  }
  comms.sendMotorData(vels, MOTOR_COUNT);
}

void loop() {
  delay(10);
  // Recieve Commands
  comms.recv();

  // Check if a full command has arrived
  if (comms.hasNewData()) {
    char type = comms.getType();
    // Execute commands depending on type
    switch (type) {
      case SerialTypes::mtr_effort_cmd:
        runMotors();
        break;
      case SerialTypes::mtr_stop_cmd:
        stopMotors();
        break;
      default:
        comms.sendError("Invalid type");
    }
  }

  // sendSensorData();
}



