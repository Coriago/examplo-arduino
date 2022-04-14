#ifndef LowPass_H
#define LowPass_H

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

class LowPass {
private:
    float a[2];
    float b[3];
    float omega0;
    float dt;
    bool adapt;
    float tn1 = 0;
    float x[3]; // Raw values
    float y[3]; // Filtered values

public:
    LowPass(float f0, float fs, bool adaptive);
    void setCoef(void);
    float filt(float xn);
};

#endif
