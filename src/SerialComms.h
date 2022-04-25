#ifndef SerialComms_H
#define SerialComms_H

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

class SerialTypes {
public:
    static const char stop_all = 'a';
    static const char error = 'b';
    static const char recieved = 'c';
    static const char mtr_snsr_data = 'd';
    static const char mtr_stop_cmd = 'e';
    static const char mtr_speed_cmd = 'f';
    static const char mtr_effort_cmd = 'g';
};

class SerialComms {
public:

    void recv(void);
    bool hasNewData(void);
    char getType(void);

    void sendMotorEffort(int efforts[], byte motorCount);
    void parseMotorEffort(int efforts[], byte motorCount);

    void sendMotorSpeed(float vels[], byte motorCount);
    void parseMotorSpeed(float vels[], byte motorCount);
    
    void sendSensorData(float vels[], byte motorCount, int battery, double volt);
    void sendError(String msg);
    void sendRecieved(void);

private:
    bool newData = false;
    char receivedChars[64];
    const byte numChars = 64;
    const char startMarker = '<';
    const char endMarker = '>';
    const long sensor_update_interval = 5e5;
    long last_sensor_update = 0;
};

#endif
