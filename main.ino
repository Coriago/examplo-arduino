#include "src/PidMotor.h"
#include "src/SerialComms.h"
#include "src/Battery.h"

const byte MOTOR_COUNT = 4;
bool pidEnabled = true;

SerialComms comms;
Battery battery(A0);
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

void runMotorsEffort() {
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

void runMotorsSpeed() {
  float vels[MOTOR_COUNT];
  comms.parseMotorSpeed(vels, MOTOR_COUNT);
  for (byte i = 0; i < MOTOR_COUNT; i++) {
    motors[i].setMotorSpeed(vels[i]);
  }
  comms.sendRecieved();
}

void stopMotors() {
  for (byte i = 0; i < MOTOR_COUNT; i++) {
    motors[i].setMotorSpeed(0);
    motors[i].setMotorSpeed(0);
    motors[i].runMotor(0, 0);
    motors[i].runMotor(0, 0);
  }
  comms.sendRecieved();
}

void sendSensorData() {
  // Voltage Sensor data
  double voltage = battery.getBatteryVoltage();
  int percentage = battery.getBatteryPercentage();

  // Motor sensor data
  float vels[MOTOR_COUNT];
  for (byte i = 0; i < MOTOR_COUNT; i++) {
    vels[i] = motors[i].getVelocity();
  }
  comms.sendSensorData(vels, MOTOR_COUNT, motors[0].getPwm(), percentage, voltage);
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
        runMotorsEffort();
        break;
      case SerialTypes::mtr_speed_cmd:
        runMotorsSpeed();
        pidEnabled = true;
        break;
      case SerialTypes::mtr_stop_cmd:
        stopMotors();
        break;
      case SerialTypes::stop_all:
        exit(0);
        break;
      default:
        comms.sendError("Invalid Type");
    }
  }

  if (pidEnabled) {
    for (byte i = 0; i < MOTOR_COUNT; i++) {
      motors[i].update();
    }
  }

  sendSensorData();
}



