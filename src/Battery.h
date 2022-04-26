#ifndef Battery_H
#define Battery_H

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

class Battery {
private:
    int percentage = 0;
    double voltage_avg = 0;
    int voltage_offset = 5;
    int _batpin;
    int counter = 0;
    int voltage_count = 15;
    double voltages[15];
    double batt_percentage[20] = {
        12.53,
        12.45,
        12.33,
        12.25,
        12.07,
        11.95,
        11.86,
        11.74,
        11.62,
        11.56,
        11.51,
        11.45,
        11.39,
        11.36,
        11.3,
        11.24,
        11.18,
        11.12,
        11.06,
        10.83
    };

public:
    Battery(int pin);
    double getBatteryVoltage(void);
    int getBatteryPercentage(void);
    void updateAvg(void);
};

#endif
