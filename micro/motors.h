/*! \file motors.h
 *  \author Soo-Hyun Yoo
 *  \brief Calculate PWM outputs for motors and servo based on target rotation or translation input.
 *
 *  Details.
 */

#ifndef MOTORS_H
#define MOTORS_H

#include "globals.h"
#include "pid.h"

// Calculate desired angular velocities based on desired angular position
// inputs.
void angular_position_controller (float* desired_pos, float* current_pos, float* desired_vel) {
    // Cap desired_pos.
    for (int i=0; i<3; i++) {
        if (desired_pos[i] > TARGET_ANG_POS_CAP) {
            desired_pos[i] = TARGET_ANG_POS_CAP;
        }
        else if (desired_pos[i] < -TARGET_ANG_POS_CAP) {
            desired_pos[i] = -TARGET_ANG_POS_CAP;
        }
    }

    // Calculate intermediate PID values that will be used later to calculate
    // the PWM outputs for the motors.
    pidAngPos[0] = updatePID(desired_pos[0], current_pos[0], PID[PID_ANG_POS_X]);
    pidAngPos[1] = updatePID(desired_pos[1], current_pos[1], PID[PID_ANG_POS_Y]);
    pidAngPos[2] = updatePID(desired_pos[2], current_pos[2], PID[PID_ANG_POS_Z]);

    for (int i=0; i<3; i++) {
        desired_vel[i] = pidAngPos[i];
    }
}

// Calculate throttle shifts for the individual motors based on desired angular
// velocity inputs.
void angular_velocity_controller (float* desired_vel, float* current_vel, int16_t* pwmShift) {
    // Cap desired_vel.
    for (int i=0; i<3; i++) {
        if (desired_vel[i] > TARGET_ANG_VEL_CAP) {
            desired_vel[i] = TARGET_ANG_VEL_CAP;
        }
        else if (desired_vel[i] < -TARGET_ANG_VEL_CAP) {
            desired_vel[i] = -TARGET_ANG_VEL_CAP;
        }
    }

    // Calculate intermediate PID values that will be used later to calculate
    // the PWM outputs for the motors.
    pidAngVel[0] = updatePID(desired_vel[0], current_vel[0], PID[PID_ANG_VEL_X]);
    pidAngVel[1] = updatePID(desired_vel[1], current_vel[1], PID[PID_ANG_VEL_Y]);
    pidAngVel[2] = updatePID(desired_vel[2], current_vel[2], PID[PID_ANG_VEL_Z]);

    for (int i=0; i<3; i++) {
        pwmShift[i] = (int16_t) pidAngVel[i];
    }
}

void calculate_pwm_outputs(float pwmThrottle, int16_t* pwmShift, int16_t* pwmOutput) {
    // ====================================================================
    // Calculate motor/servo values.
    //     MOTOR_X_OFFSET: Offset starting motor values to account for
    //                     chassis imbalance.
    //     MOTOR_X_SCALE: Scale targetRot.
    //     joy.axes[SZ]: Throttle.
    //
    // TODO: MOTOR_X_SCALE should be replaced by actual PID gains. Besides,
    // it's incorrect to scale in the negative direction if the
    // corresponding arm is heavier.
    // TODO: The last term for each pwmOutput is INACCURATE. Fix this.
    // ====================================================================
    pwmOutput[THRUSTER_R] = TNEUTRAL + pwmThrottle + pwmShift[2];
    pwmOutput[THRUSTER_L] = TNEUTRAL + pwmThrottle - pwmShift[2];

    pwmOutput[THRUSTER_FR] = TNEUTRAL + pwmShift[0];
    pwmOutput[THRUSTER_FL] = TNEUTRAL + pwmShift[0];

    pwmOutput[THRUSTER_BR] = TNEUTRAL - pwmShift[0];
    pwmOutput[THRUSTER_BR] = TNEUTRAL - pwmShift[0];


    // TODO: Offsets and scales.
    //pwmOutput[MOTOR_T] = TMIN + MOTOR_T_OFFSET + MOTOR_T_SCALE * pwmOutput[MOTOR_T];
    //pwmOutput[MOTOR_R] = TMIN + MOTOR_R_OFFSET + MOTOR_R_SCALE * pwmOutput[MOTOR_R];
    //pwmOutput[MOTOR_L] = TMIN + MOTOR_L_OFFSET + MOTOR_L_SCALE * pwmOutput[MOTOR_L];

    // ====================================================================
    // After finding the maximum and minimum motor values, limit, but NOT
    // fit, motor values to minimum and maximum throttle [TMIN, TMAX]).
    // Doing this incorrectly will result in motor values seemingly stuck
    // mostly at either extremes.
    // ====================================================================
    int mapUpper = pwmOutput[THRUSTER_R] > pwmOutput[THRUSTER_FR] ? pwmOutput[THRUSTER_R] : pwmOutput[THRUSTER_FR];
    mapUpper = mapUpper > pwmOutput[THRUSTER_FL] ? mapUpper : pwmOutput[THRUSTER_FL];
    mapUpper = mapUpper > pwmOutput[THRUSTER_L] ? mapUpper : pwmOutput[THRUSTER_L];
    mapUpper = mapUpper > pwmOutput[THRUSTER_BL] ? mapUpper : pwmOutput[THRUSTER_BL];
    mapUpper = mapUpper > pwmOutput[THRUSTER_BR] ? mapUpper : pwmOutput[THRUSTER_BR];
    mapUpper = mapUpper > TMAX ? mapUpper : TMAX;

    int mapLower = pwmOutput[THRUSTER_R] < pwmOutput[THRUSTER_FR] ? pwmOutput[THRUSTER_R] : pwmOutput[THRUSTER_FR];
    mapLower = mapLower < pwmOutput[THRUSTER_FL] ? mapLower : pwmOutput[THRUSTER_FL];
    mapLower = mapLower < pwmOutput[THRUSTER_L] ? mapLower : pwmOutput[THRUSTER_L];
    mapLower = mapLower < pwmOutput[THRUSTER_BL] ? mapLower : pwmOutput[THRUSTER_BL];
    mapLower = mapLower < pwmOutput[THRUSTER_BR] ? mapLower : pwmOutput[THRUSTER_BR];
    mapLower = mapLower < TMAX ? mapLower : TMAX;

    // We shouldn't have to use these, but uncomment the following two
    // lines if pwmOutput goes crazy and makes mapUpper lower than mapLower:
    //mapUpper = mapUpper > TMIN ? mapUpper : TMIN+1;
    //mapLower = mapLower < TMAX ? mapLower : TMAX-1;

    // ====================================================================
    // If map bounds are reasonable, remap range to [mapLower, mapUpper].
    // Otherwise, kill motors. Note that map(), an Arduino function, does
    // integer math and truncates fractions.
    // ====================================================================
    for (int i=0; i<6; i++) {
        if (mapUpper > mapLower) {
            pwmOutput[i] = map(pwmOutput[i], mapLower, mapUpper, TMIN, TMAX);
        }
        else {
            pwmOutput[i] = TNEUTRAL;
        }
    }
}

#endif // MOTORS_H

