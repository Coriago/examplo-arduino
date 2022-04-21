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

    void sendMotorEffort(int efforts[], byte motorNumber);
    void parseMotorEffort(int efforts[], byte motorNumber);
    
    void sendMotorData(float vels[], byte motorCount);
    void sendError(String msg);
    void sendRecieved(void);

private:
    bool newData = false;
    char receivedChars[64];
    const byte numChars = 64;
    const char startMarker = '<';
    const char endMarker = '>';

};

#endif
