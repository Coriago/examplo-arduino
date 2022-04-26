#include "Battery.h"

Battery::Battery(int pin) {
    _batpin = pin;
}

void Battery::updateAvg(void) {
    int volt = analogRead(_batpin);
    double voltage = map(volt,0,1023, 0, 2500) + voltage_offset;
    voltage /= 100;
    voltages[counter] = voltage;
    
    if (counter == voltage_count - 1) {
        counter = 0;
    } else {
        counter += 1;
    }

    double sum = 0;
    for (int i = 0; i < voltage_count; i++) {
        sum += voltages[i];
    }

    voltage_avg = sum / voltage_count;
}

double Battery::getBatteryVoltage(void) {
    this->updateAvg();
    return voltage_avg;
}

int Battery::getBatteryPercentage(void) {
    // this->updateAvg();
    for (int i = 0; i < 20; i++) {
        if (voltage_avg >= batt_percentage[i]) {
            return (20 - i) * 5;
        }
    }
    return 0;
}