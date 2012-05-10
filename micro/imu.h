/*! \file imu.h
 *  \author Soo-Hyun Yoo
 *  \brief Header for IMU class.
 *
 *  Details.
 */

#ifndef IMU_H
#define IMU_H

#include "itg3200.cpp"
#include "adxl345.cpp"
#include "hmc5883.cpp"
#include "rov_math.h"
#include "globals.h"

#define ACC_WEIGHT 0.050   // Accelerometer data weight relative to gyro's weight of 1
//#define ACC_SCALE_WEIGHT 30   // Reduce accelerometer weight if measured magnitude of acceleration differs significantly from 1 g.
//#define ACC_SELF_WEIGHT 0.02   // The weight of the newest measurement in the weighted average of accumulated and newest measurements.
//#define ACC_UPDATE_SIG 50.0   // Update sigma for Kalman filter
//#define ACC_PREDICT_SIG 10.2   // Prediction sigma for Kalman filter

//#define MAG_WEIGHT 0.05


class IMU {
    ADXL345 acc;
    ITG3200 gyro;
    HMC5883 mag;

    float accScale;
    float accWeight;   // Variable accelerometer weight.
    //float accVar;   // Accelerometer variance for Kalman filter.

    float aVec[3];   // Accelerometer read.
    float aVecLast[3];   // Last accelerometer read.
    float gVec[3];   // Gyro read.
    float mVec[3];   // Magnetometer read.

    float kbb[3];   // K body unit vector expressed in body coordinates.
    float kgb[3];   // K global unit vector expressed in body coordinates.
    float jgb[3];   // J global unit vector expressed in body coordinates.

    float wA[3];   // Corrective rotation vector based on acceleration vector.
    float wM[3];   // Corrective rotation vector based on magnetic north vector.
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

