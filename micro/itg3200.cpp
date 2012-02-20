/*! \file itg3200.cpp
 *  \author Soo-Hyun Yoo
 *  \brief Source for ITG3200 gyroscope class.
 *
 *  Details.
 */

#include "itg3200.h"

ITG3200::ITG3200() {
    readI2C(GYRADDR, 0x00, 1, buffer);   // Who am I?

    sp("ITG-3200 ID = ");
    spln((int) buffer[0]);

    // Configure ITG-3200
    // Refer to DS Section 8: Register Description.
    sendI2C(GYRADDR, 0x15, 0x18);   // 00011000 -- Sample rate divider is 24(+1), so 40 Hz

    // Set FS_SEL (operation range) to 3 as recommended on DS p. 24.
    readI2C(GYRADDR, 0x16, 1, buffer);
    buffer[1] = 3;
    buffer[1] = (buffer[1] << 3);   // FS_SEL is on bits 4 and 3.
    buffer[0] |= buffer[1];
    sendI2C(GYRADDR, 0x16, buffer[0]);

    spln("ITG-3200 configured!");

    // Zero buffer.
    for (int i=0; i<6; i++) {
        buffer[i] = 0;
    }

    // For Runge-Kutta integration.
    rkIndex = 0;
    for (int i=0; i<3; i++)
        for (int j=0; j<4; j++)
            rkVal[i][j] = 0;

    for (int i=0; i<3; i++) {
        tempData[i] = 0;
        gZero[i] = 0;
        gVec[i] = 0;
        angle[i] = 0;
    }
    calibrated = false;
}

void ITG3200::calibrate(int sampleNum) {
    for (int i=0; i<sampleNum; i++) {
        ITG3200::poll();
        for (int j=0; j<3; j++) {
            tempData[j] = tempData[j] + gVec[j];
        }
    }

    gZero[0] = tempData[0]/sampleNum;
    gZero[1] = tempData[1]/sampleNum;
    gZero[2] = tempData[2]/sampleNum;

    spln("Gyro calibration complete!");
    sp(tempData[0]/sampleNum);
    calibrated = true;
}

void ITG3200::poll() {
    readI2C(GYRADDR, 0x1d, 6, buffer);

    // Shift high byte to be high 8 bits and append with low byte.
    gRaw[1] = ((buffer[0] << 8) | buffer[1]);   // Tricopter Y axis is chip X axis.
    gRaw[0] = ((buffer[2] << 8) | buffer[3]);   // Tricopter X axis is chip Y axis. Must be negated later!
    gRaw[2] = ((buffer[4] << 8) | buffer[5]);   // Z axis is same.

    sp("G( ");
    for (int i=0; i<3; i++) {
        sp(gRaw[i]);
        sp(" ");
    }
    spln(")");


    // Read gyro temperature.
    //readI2C(GYRADDR, TEMP_OUT, 2, buffer);
    //temp = 35 + (((buffer[0] << 8) | buffer[1]) + 13200)/280.0;

    // Convert raw gyro output values to rad/s.
    // Output: [0x0000 -- 0x7fff] = [     0 -- 32767]
    //         [0x8000 -- 0xffff] = [-32768 --    -1]
    // Range: [-2000, 2000] deg/s
    // Scale factor: 14.375 LSB / (deg/s)
    for (int i=0; i<3; i++) {
        float tmp;
        if (gRaw[i] >= 0x8000)   // If zero to negative rot. vel.: 2^16-1 to 2^15...
            tmp = -((signed) (0x10000 - gRaw[i]));   // ...subtract from 2^16.
        else
            tmp = gRaw[i];   // Otherwise, leave it alone.
        switch (i) {
            case 0: gVec[0] = -tmp / 14.375 * PI/180; break;
            case 1: gVec[1] =  tmp / 14.375 * PI/180; break;
            case 2: gVec[2] =  tmp / 14.375 * PI/180; break;
            default: break;
        }
        gVec[i] = (gVec[i] - gZero[i]);
        //if (abs(gVec[i]*1000) < 7) gVec[i] = 0;
        //sp(gVec[i]);
        //sp(" ");
    }

    // Runge-Kutta smoothing.
    #ifdef ENABLE_GYRO_RK_SMOOTH
    if (calibrated) {
        for (int i=0; i<3; i++) {
            rkVal[i][rkIndex] = gVec[i];
            gVec[i] = (1*rkVal[i][rkIndex] + 
                       2*rkVal[i][(rkIndex+1)%4] +
                       2*rkVal[i][(rkIndex+2)%4] +
                       1*rkVal[i][(rkIndex+3)%4])/6;
        }
        rkIndex = (rkIndex + 1) % 4;   // Increment index by 1 but loop back from 3 back to 0.
    }
    #endif // ENABLE_GYRO_RK_SMOOTH
}

float* ITG3200::get() {
    return gVec;
}

float ITG3200::get(int axis) {
    return gVec[axis];
}

