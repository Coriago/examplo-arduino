#include "LowPass.h"

LowPass::LowPass(float f0, float fs, bool adaptive){
    // f0: cutoff frequency (Hz)
    // fs: sample frequency (Hz)
    // adaptive: boolean flag, if set to 1, the code will automatically set
    // the sample frequency based on the time history.
    
    omega0 = 6.28318530718*f0;
    dt = 1.0/fs;
    adapt = adaptive;
    tn1 = -dt;
    for(int k = 0; k < 3; k++){
        x[k] = 0;
        y[k] = 0;        
    }
    setCoef();
};

void LowPass::setCoef(void){
    if(adapt){
        float t = micros()/1.0e6;
        dt = t - tn1;
        tn1 = t;
    }
    float alpha = omega0*dt;
    float c1 = 2*sqrt(2)/alpha;
    float c2 = 4/(alpha*alpha);
    float denom = 1.0+c1+c2;
    b[0] = 1.0/denom;
    b[1] = 2.0/denom;
    b[2] = b[0];
    a[0] = -(2.0-2.0*c2)/denom;
    a[1] = -(1.0-c1+c2)/(1.0+c1+c2);      
};

float LowPass::filt(float xn){
    // Provide me with the current raw value: x
    // I will give you the current filtered value: y
    if(adapt){
        setCoef(); // Update coefficients if necessary      
    }
    y[0] = 0;
    x[0] = xn;
    // Compute the filtered values
    for(int k = 0; k < 2; k++){
        y[0] += a[k]*y[k+1] + b[k]*x[k];
    }
    y[0] += b[2]*x[2];

    // Save the historical values
    for(int k = 2; k > 0; k--){
        y[k] = y[k-1];
        x[k] = x[k-1];
    }

    // Return the filtered value    
    return y[0];
};