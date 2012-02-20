/*! \file adxl345.cpp
 *  \author Soo-Hyun Yoo
 *  \brief Source for ADXL345 accelerometer class.
 *
 *  Details.
 */

#include "adxl345.h"

ADXL345::ADXL345(uint8_t range, uint8_t bw) {
    readI2C(ACCADDR, 0x00, 1, buffer);

    sp("ADXL345 ID = ");
    spln((int) buffer[0]);

    // Set ee_w bit
    readI2C(ACCADDR, CTRLREG0, 1, buffer);
    buffer[0] |= 0x10;   // Bitwise OR operator to set ee_w bit.
    sendI2C(ACCADDR, CTRLREG0, buffer[0]);   // Have to set ee_w to write any other registers.

    // Set range.
    readI2C(ACCADDR, OLSB1, 1, buffer);
    buffer[1] = range;
    buffer[1] = (buffer[1] << 1);   // Need to shift left one bit; refer to DS p. 21.
    buffer[0] &= (~0x0e);   // Clear old range bits.
    buffer[0] |= buffer[1];
    sendI2C(ACCADDR, OLSB1, buffer[0]);   // Write new range data, keep other bits the same.

    // Set ADC resolution (DS p. 8).
    res = 0.125;                           // [   -1,   1] g
    if      (range == 1) res *= 1.5;   // [ -1.5, 1.5] g
    else if (range == 2) res *= 2;     // [   -2,   2] g
    else if (range == 3) res *= 3;     // [   -3,   3] g
    else if (range == 4) res *= 4;     // [   -4,   4] g
    else if (range == 5) res *= 8;     // [   -8,   8] g
    else if (range == 6) res *= 16;    // [  -16,  16] g

    // Set bandwidth.
    //     bw  bandwidth (Hz)
    //      0              10
    //      1              20
    //      2              40
    //      3              75
    //      4             150
    //      5             300
    //      6             600
    //      7            1200
    readI2C(ACCADDR, BWTCS, 1, buffer);
    buffer[1] = bw;
    buffer[1] = (buffer[1] << 4);   // Need to shift left four bits; refer to DS p. 21.
    buffer[0] &= (~0xf0);   // Clear bandwidth bits <7:4>.
    buffer[0] |= buffer[1];
    sendI2C(ACCADDR, BWTCS, buffer[0]);   // Keep tcs<3:0> in BWTCS, but write new BW.

    // Set mode_config to 0x01 (ultra low noise mode, DS p. 28).
    //readI2C(ACCADDR, 0x30, 1, buffer);
    //buffer[0] &= (~0x03);   // Clear mode_config bits <1:0>.
    //buffer[0] |= 0x01;
    //sendI2C(ACCADDR, 0x30, buffer[0]);

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
    readI2C(ACCADDR, 0x02, 6, buffer);

    aRaw[1] = ((buffer[1] << 6) | (buffer[0] >> 2));   // Tricopter Y axis is chip X axis.
    aRaw[0] = ((buffer[3] << 6) | (buffer[2] >> 2));   // Tricopter X axis is chip Y axis. Must be negated later!
    aRaw[2] = ((buffer[5] << 6) | (buffer[4] >> 2));   // Z axis is same.

    // Read accelerometer temperature.
    //readI2C(ACCADDR, 0x08, 1, buffer);
    //temp = -40 + 0.5 * buffer[0];   // 0.5 K/LSB

    // Convert raw values to multiples of gravitational acceleration.
    // Output: [0x1fff -- 0x0000] = [-8191 --    0]
    //         [0x3fff -- 0x2000] = [    1 -- 8192]
    // ADC resolution varies depending on setup. See DS p. 27 or the
    // constructor of this class.
    for (int i=0; i<3; i++) {
        float tmp;

        if (aRaw[i] < 0x2000)
            tmp = -((signed) aRaw[i]);
        else
            tmp = 0x4000 - aRaw[i];

        aVec[i] = tmp * res;
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

