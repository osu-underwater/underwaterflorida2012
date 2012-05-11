/*! \file micro.ino
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief ROV main controller loop.
 */

//#include <Wire.h>
#include "ethcomm.cpp"
#include "imu.cpp"
#include "pilot.cpp"
#include "telemetry.h"

int main(void) {
    init();   // For Arduino.

    // Begin Arduino services.
    Wire.begin();

    Serial.begin(115200);

    EthComm eth;
    Pilot pilot;

    IMU imu;
    imu.init();

    Serial.println("Setup complete.");

    Servo pwmDevice[6];
    pwmDevice[0].attach(THRUSTER_R);
    pwmDevice[1].attach(THRUSTER_FR);
    pwmDevice[2].attach(THRUSTER_FL);
    pwmDevice[3].attach(THRUSTER_L);
    pwmDevice[4].attach(THRUSTER_BL);
    pwmDevice[5].attach(THRUSTER_BR);

    uint64_t nextRunTime = micros();
    loopCount = 0;

    while (1) {
        if (micros() >= nextRunTime) {
            // ================================================================
            // System loop
            // ================================================================
            imu.update();   // Run this ASAP when loop starts so gyro integration is as accurate as possible.
            nextRunTime += MASTER_DT;   // Update next loop start time.

            // ================================================================
            // Control loop
            // ================================================================
            if (loopCount % CONTROL_LOOP_INTERVAL == 0) {
                pilot.fly();

                for (int i=0; i<6; i++) {
                    pwmDevice[i].writeMicroseconds(pwmOut[i]);
                }
            }

            // ================================================================
            // Telemetry loop
            // ================================================================
            if (loopCount % COMM_LOOP_INTERVAL == 0) {
                sendTelemetry(nextRunTime);
                eth.test();
            }

            loopCount++;
            loopCount = loopCount % 1000;
        }
    }

    return 0;
}

