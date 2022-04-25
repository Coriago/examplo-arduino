#include "src/PidMotor.h"
#include "src/SerialComms.h"

const byte MOTOR_COUNT = 4;
bool pidEnabled = true;

int voltage_offset = 5;
double voltage_avg = 0;
int voltage_count = 0;
int avg_count = 15;
int percentage = 0;
double batt_percentage[20] = {12.6, 12.45, 12.33, 12.25, 12.07, 11.95, 11.86, 11.74, 11.62, 11.56, 11.51, 11.45, 11.39, 11.36, 11.3, 11.24, 11.18, 11.12, 11.06, 10.83};


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
    motors[i].runMotor(0, 0);
  }
  comms.sendRecieved();
}

int getBatteryPercentage(double volt) {
  for (int i = 0; i < 20; i++) {
    if (volt >= batt_percentage[i]) {
      return (20 - i) * 5;
    }
  }
  return 0;
}

void sendSensorData() {
  // Voltage Sensor data
  int volt = analogRead(A0);
  double voltage = map(volt,0,1023, 0, 2500) + voltage_offset;
  voltage /= 100;
  voltage_avg += voltage;
  voltage_count += 1;

  if (voltage_count == avg_count) {
    voltage_avg /= avg_count;
    percentage = getBatteryPercentage(voltage_avg);
    voltage_count = 0;
    voltage_avg = 0;
  }

  // Motor sensor data
  float vels[MOTOR_COUNT];
  for (byte i = 0; i < MOTOR_COUNT; i++) {
    vels[i] = motors[i].getVelocity();
  }
  comms.sendSensorData
  (vels, MOTOR_COUNT, percentage, voltage);
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



