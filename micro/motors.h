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
    for (int i=0; i<2; i++) {
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

    //digitalWrite(13, HIGH);
    //spln((int) (1000000*pidAngPos[0]));
    //digitalWrite(13, LOW);
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
    // ====================================================================
    pwmOutput[THRUSTER_R] = TNEUTRAL + pwmThrottle - pwmShift[2];
    pwmOutput[THRUSTER_L] = TNEUTRAL + pwmThrottle + pwmShift[2];

    // TODO: input_axes[5] controls vertical movement of ROV. This is a hack!
    pwmOutput[THRUSTER_FR] = SBMIN + pwmShift[0]*10 - input_axes[5]*100;
    pwmOutput[THRUSTER_FL] = SBMIN + pwmShift[0]*10 - input_axes[5]*100;
    pwmOutput[THRUSTER_BL] = SBMIN - pwmShift[0]*10 - input_axes[5]*100;
    pwmOutput[THRUSTER_BR] = SBMIN - pwmShift[0]*10 - input_axes[5]*100;

    digOut[THRUSTER_FR_DIG] = (pwmOutput[THRUSTER_FR] > 0) ? 1 : 0;
    digOut[THRUSTER_FL_DIG] = (pwmOutput[THRUSTER_FL] > 0) ? 1 : 0;
    digOut[THRUSTER_BL_DIG] = (pwmOutput[THRUSTER_BL] > 0) ? 1 : 0;
    digOut[THRUSTER_BR_DIG] = (pwmOutput[THRUSTER_BR] > 0) ? 1 : 0;

    pwmOutput[THRUSTER_FR] = ABS(pwmOutput[THRUSTER_FR]);
    pwmOutput[THRUSTER_FL] = ABS(pwmOutput[THRUSTER_FL]);
    pwmOutput[THRUSTER_BL] = ABS(pwmOutput[THRUSTER_BL]);
    pwmOutput[THRUSTER_BR] = ABS(pwmOutput[THRUSTER_BR]);


    int mapUpper, mapLower;

    // Cap PWM values for CrustCrawler thrusters.
    mapUpper = pwmOutput[THRUSTER_R] > pwmOutput[THRUSTER_L] ? pwmOutput[THRUSTER_R] : pwmOutput[THRUSTER_L];
    mapUpper = mapUpper > TMAX ? mapUpper : TMAX;

    mapLower = pwmOutput[THRUSTER_R] < pwmOutput[THRUSTER_L] ? pwmOutput[THRUSTER_R] : pwmOutput[THRUSTER_L];
    mapLower = mapLower < TMIN ? mapLower : TMIN;

    if (mapUpper > mapLower) {
        pwmOutput[THRUSTER_R] = map(pwmOutput[THRUSTER_R], mapLower, mapUpper, TMIN, TMAX);
        pwmOutput[THRUSTER_L] = map(pwmOutput[THRUSTER_L], mapLower, mapUpper, TMIN, TMAX);
    }
    else {
        pwmOutput[THRUSTER_R] = TNEUTRAL;
        pwmOutput[THRUSTER_L] = TNEUTRAL;
    }

    // Cap PWM values for SeaBotics thrusters.
    mapUpper = pwmOutput[THRUSTER_FR] > pwmOutput[THRUSTER_FL] ? pwmOutput[THRUSTER_FR] : pwmOutput[THRUSTER_FL];
    mapUpper = mapUpper > pwmOutput[THRUSTER_BL] ? mapUpper : pwmOutput[THRUSTER_BL];
    mapUpper = mapUpper > pwmOutput[THRUSTER_BR] ? mapUpper : pwmOutput[THRUSTER_BR];
    mapUpper = mapUpper > SBMAX ? mapUpper : SBMAX;

    mapLower = pwmOutput[THRUSTER_FR] < pwmOutput[THRUSTER_FL] ? pwmOutput[THRUSTER_FR] : pwmOutput[THRUSTER_FL];
    mapLower = mapLower < pwmOutput[THRUSTER_BL] ? mapLower : pwmOutput[THRUSTER_BL];
    mapLower = mapLower < pwmOutput[THRUSTER_BR] ? mapLower : pwmOutput[THRUSTER_BR];
    mapLower = mapLower < SBMIN ? mapLower : SBMIN;

    if (mapUpper > mapLower) {
        pwmOutput[THRUSTER_FR] = map(pwmOutput[THRUSTER_FR], mapLower, mapUpper, SBMIN, SBMAX);
        pwmOutput[THRUSTER_FL] = map(pwmOutput[THRUSTER_FL], mapLower, mapUpper, SBMIN, SBMAX);
        pwmOutput[THRUSTER_BL] = map(pwmOutput[THRUSTER_BL], mapLower, mapUpper, SBMIN, SBMAX);
        pwmOutput[THRUSTER_BR] = map(pwmOutput[THRUSTER_BR], mapLower, mapUpper, SBMIN, SBMAX);
    }
    else {
        pwmOutput[THRUSTER_FR] = SBMIN;
        pwmOutput[THRUSTER_FL] = SBMIN;
        pwmOutput[THRUSTER_BL] = SBMIN;
        pwmOutput[THRUSTER_BR] = SBMIN;
    }
}

#endif // MOTORS_H

