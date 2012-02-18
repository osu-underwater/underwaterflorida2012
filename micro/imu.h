/*! \file imu.h
 *  \author Soo-Hyun Yoo
 *  \brief Header for IMU class.
 *
 *  Details.
 */

#ifndef IMU_H
#define IMU_H

#include "itg3200.cpp"
#include "bma180.cpp"
#include "triMath.h"
#include "globals.h"

#define ACC_WEIGHT 0.012   // Accelerometer data weight relative to gyro's weight of 1

class IMU {
    BMA180 myAcc;
    ITG3200 myGyr;

    float aVec[3];   // Accelerometer output.
    float gVec[3];   // Gyro output.

    // TODO: Implement variable weight coefficient for accelerometer based on
    // total observed acceleration.
    float kbb[3];   // K body unit vector expressed in body coordinates.
    float kgb[3];   // K global unit vector expressed in body coordinates.
    float wA[3];    // Corrective rotation vector based on acceleration vector.
    float wAOffset[3];   // Correction vector for wA.
    float wdt[3];    // Angular displacement vector = w * dt, where w is the angular velocity vector and dt is the time elapsed.

    float gyroDCM[3][3];     // DCM based on gyro readings, corrected with wA.
    float offsetDCM[3][3];   // DCM used to offset gyroDCM to produce bodyDCM.
    float dDCM[3][3];   // First used to store the change in DCM to update the current DCM. Repurposed during orthonormalization to store the correction vectors for the i and j unit vectors.
    float errDCM;   // DCM error for which we need orthonormalization.

    void orthonormalize(float[3][3]);

public:
    IMU();
    void init();
    void update();
    void reset();
};

#endif // IMU_H

