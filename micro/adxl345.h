/*! \file adxl345.h
 *  \author Soo-Hyun Yoo
 *  \brief Header file for ADXL345 accelerometer class.
 *
 *  Details.
 */

#ifndef ADXL345_H
#define ADXL345_H

#include "i2c.h"
#include "globals.h"

//#define ENABLE_ACC_RK_SMOOTH   // Enable Runge-Kutta smoothing (low-pass filter)

#define ACC_ADDR 0xe5   // ADXL345 device address.

class ADXL345 {
    uint8_t buffer[6];   // Buffer to which we save data read from device
    char aStr[512];   // String buffer to organize data before sending to serial line
    uint16_t aRaw[3];   // Raw digital values.
    float aVec[3];   // In g's
    float temp;   // Temperature.

    int rkIndex;
    float rkVal[3][4];   // Four Runge-Kutta integrator values for each of three axes

public:
    ADXL345(uint8_t, uint8_t);
    void poll();
    float* get();
    float get(int);
};

#endif // ADXL345_H

