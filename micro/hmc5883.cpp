/*! \file hmc5883.cpp
 *  \author Soo-Hyun Yoo
 *  \brief Source for HMC5883 magnetometer class.
 *
 *  Details.
 */

#include "hmc5883.h"

HMC5883::HMC5883() {
    // Put into continuous measurement mode (DS p. 12).
    sendI2C(MAG_ADDRESS, 0x02, 0x00);
}

void HMC5883::poll() {
    // Read data.
    readI2C(MAG_ADDRESS, 0x03, 6, buffer);

    // Read in high and low bytes.
    mRaw[0] = ((buffer[0] << 8) | buffer[1]);   // X
    mRaw[1] = ((buffer[2] << 8) | buffer[3]);   // Y
    mRaw[2] = ((buffer[4] << 8) | buffer[5]);   // Z

    //sp("M( ");
    //for (int i=0; i<3; i++) {
    //    sp(mRaw[i]);
    //    sp(" ");
    //}
    //spln(")   ");

    // Convert raw outputs to vector values.
    // Output: [0xf800 -- 0xffff] = [-2048 --   -1]
    //         [0x0000 -- 0x07ff] = [    0 -- 2047]
    for (int i=0; i<3; i++) {
        float tmp;

        if (mRaw[i] >= 0xf800)
            tmp = -((signed) (0x10000 - mRaw[i]));
        else
            tmp = mRaw[i];

        mVec[i] = tmp;
    }
    vNorm(mVec);   // Normalize.
}

float* HMC5883::get() {
    return mVec;
}

float HMC5883::get(int axis) {
    return mVec[axis];
}

