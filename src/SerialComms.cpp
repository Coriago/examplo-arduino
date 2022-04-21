#include "SerialComms.h"

void SerialComms::recv(void) {
    static bool recvInProgress = false;
    static byte ndx = 0;
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

bool SerialComms::hasNewData(void) {
    return newData;
}

char SerialComms::getType(void) {
    return receivedChars[0];
};

void SerialComms::sendMotorEffort(int efforts[], byte motorNumber) {
    String cmd = String(SerialTypes::mtr_effort_cmd);
    for(int i = 0; i < motorNumber-1; i++) {
        cmd += String(efforts[i]);
        cmd += " ";
    }
    cmd += efforts[motorNumber-1];
    Serial.println(cmd);
};

void SerialComms::parseMotorEffort(int efforts[], byte motorCount) {
    char tempChars[numChars];
    strcpy(tempChars, receivedChars);
    char * strtoIndx;

    // Skip the type char
    strtoIndx = strtok(tempChars, ",");

    for (byte i = 0; i < motorCount; i++) {
        strtoIndx = strtok(NULL, ",");
        efforts[i] = atoi(strtoIndx);
    }
};


void SerialComms::sendMotorData(float vels[], byte motorCount) {
    String data = String(startMarker);
    data += String(SerialTypes::mtr_snsr_data);
    data += ",";
    for (byte i = 0; i < motorCount - 1; i++) {
        data += String(vels[i]);
        data += ",";
    }
    data += String(vels[motorCount - 1]);
    data += String(endMarker);
    Serial.println(data);
}

void SerialComms::sendRecieved(void) {
    String data = String(startMarker);
    data += String(SerialTypes::recieved);
    data += String(endMarker);
    Serial.println(data);
    newData = false;
}

void SerialComms::sendError(String msg) {
    String data = String(startMarker);
    data += String(SerialTypes::error);
    data += ",";
    data += msg;
    data += String(endMarker);
    Serial.println(data);
    newData = false;
}