/*! \file micro.ino
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief ROV main controller loop.
 */

//#include <Wire.h>
#include "ethcomm.cpp"
#include "imu.cpp"
#include "telemetry.h"

int main(void) {
    init();   // For Arduino.

    // Begin Arduino services.
    Wire.begin();

    Serial.begin(115200);

    //EthComm eth;

    IMU imu;
    imu.init();

    Serial.println("Setup complete.");

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
                // UPDATE PINS HERE
            }

            // ================================================================
            // Telemetry loop
            // ================================================================
            if (loopCount % TELEMETRY_LOOP_INTERVAL == 0) {
                sendTelemetry(nextRunTime);
                //eth.test();
            }

            loopCount++;
            loopCount = loopCount % 1000;
        }
    }

    return 0;
}

