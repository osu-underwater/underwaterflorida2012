/*! \file adxl345.cpp
 *  \author Soo-Hyun Yoo
 *  \brief Source for ADXL345 accelerometer class.
 *
 *  Details.
 */

#include "adxl345.h"

ADXL345::ADXL345(uint8_t range, uint8_t bw) {
    readI2C(ACC_ADDR, 0x00, 1, buffer);

    sp("ADXL345 ID = ");
    spln((int) buffer[0]);

    // Set POWER_CTL to 'measure' to turn it on.
    sendI2C(ACC_ADDR, 0x2d, 0x08);

    // Set range (DS p. 17).
    // Resolution: 256 LSB/g if FULL_RES is set.
    //     range  +/- g range
    //         0            2
    //         1            4
    //         2            8
    //         3           16
    readI2C(ACC_ADDR, 0x31, 1, buffer);
    buffer[0] &= (~0x03);   // Clear old range bits.
    buffer[0] |= (1 << 3);   // Set FULL_RES to 1.
    buffer[0] |= range;
    sendI2C(ACC_ADDR, 0x31, buffer[0]);   // Write new range data, keep other bits the same.

    // Set bandwidth (in normal mode, DS p. 6).
    //     bw  bandwidth (Hz)
    //      6               6.25
    //      7              12.5
    //      8              25
    //      9              50
    //     10             100
    //     11             200
    //     12             400
    //     13             800
    //     14            1600
    //     15            3200
    sendI2C(ACC_ADDR, 0x2c, bw);   // Set bandwidth in <3:0>, set all else to 0 (DS p. 16).

    spln("ADXL345 configured!");

    for (int i=0; i<3; i++) {
        aRaw[i] = 0;
        aVec[i] = 0;
    }

    // Zero buffer.
    for (int i=0; i<6; i++) {
        buffer[i] = 0;
    }

    rkIndex = 0;
    for (int i=0; i<3; i++)
        for (int j=0; j<4; j++)
            rkVal[i][j] = 0;
}

void ADXL345::poll() {
    // Read data.
    readI2C(ACC_ADDR, 0x32, 6, buffer);

    aRaw[0] = ((buffer[3] << 8) | buffer[2]);   // ROV X axis is negative chip Y axis. Must be negated later!
    aRaw[1] = ((buffer[1] << 8) | buffer[0]);   // ROV Y axis is negative chip X axis. Must be negated later!
    aRaw[2] = ((buffer[5] << 8) | buffer[4]);   // Z axis is the same.

    sp("A( ");
    for (int i=0; i<3; i++) {
        sp((int) aRaw[i]);
        sp(" ");
    }
    sp(")   ");

    // Convert raw values to multiples of gravitational acceleration.
    // Output: [0x1fff -- 0x0000] = [-8191 --    0]
    //         [0x3fff -- 0x2000] = [    1 -- 8192]
    // ADC resolution: 256 LSB/g.
    for (int i=0; i<3; i++) {
        float tmp;

        if (aRaw[i] < 0x2000)
            tmp = -((signed) aRaw[i]);
        else
            tmp = 0x4000 - aRaw[i];

        aVec[i] = tmp / 256;
    }
    aVec[0] *= -1;   // Negated.

    // Runge-Kutta smoothing.
    #ifdef ENABLE_ACC_RK_SMOOTH
    for (int i=0; i<3; i++) {
        rkVal[i][rkIndex] = aVec[i];
        aVec[i] = (1*rkVal[i][rkIndex] +
                2*rkVal[i][(rkIndex+1)%4] +
                2*rkVal[i][(rkIndex+2)%4] +
                1*rkVal[i][(rkIndex+3)%4])/6;
    }
    rkIndex = (rkIndex + 1) % 4;   // Increment index by 1 but loop back from 3 back to 0.
    #endif // ENABLE_ACC_RK_SMOOTH
}

float* ADXL345::get() {
    return aVec;   // In g's.
}

float ADXL345::get(int axis) {
    return aVec[axis];
}

