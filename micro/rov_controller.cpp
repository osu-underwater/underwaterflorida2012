/*! \file micro.ino
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief ROV main controller loop.
 */

//#include <Wire.h>
//#include "ethcomm.cpp"
#include "imu.cpp"
#include "pilot.cpp"
#include "telemetry.h"

int main(void) {
    init();   // For Arduino.

    // Begin Arduino services.
    Wire.begin();

    Serial.begin(115200);

    //EthComm eth;
    uint8_t mac[]     = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed};
    uint8_t ip[]      = {192,169,0,177};
    uint8_t gateway[] = {192,169,0,1};
    uint8_t subnet[]  = {255,255,255,0};
    Ethernet.begin(mac, ip, gateway, subnet);

    int pin, val;
    uint8_t myByte;
    Server server(23);
    server.begin();
    Client client = server.available();

    Pilot pilot;

    IMU imu;
    imu.init();

    Serial.println("Setup complete.");

    Servo pwmDevice[2];
    pwmDevice[0].attach(PIN_R);
    pwmDevice[1].attach(PIN_L);
    //pwmDevice[1].attach(PIN_FR);
    //pwmDevice[2].attach(PIN_FL);
    //pwmDevice[4].attach(PIN_BL);
    //pwmDevice[5].attach(PIN_BR);

    pinMode(PIN_R_EN, OUTPUT);
    pinMode(PIN_L_EN, OUTPUT);
    pinMode(PIN_FR_DIR, OUTPUT);
    pinMode(PIN_FL_DIR, OUTPUT);
    pinMode(PIN_BL_DIR, OUTPUT);
    pinMode(PIN_BR_DIR, OUTPUT);

    // Initialize CC thrusters safely.
    pwmDevice[0].writeMicroseconds(TNEUTRAL);
    pwmDevice[1].writeMicroseconds(TNEUTRAL);
    digitalWrite(PIN_R_EN, HIGH);
    digitalWrite(PIN_L_EN, HIGH);

    Timer3.initialize(50);

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

                pwmDevice[0].writeMicroseconds(pwmOut[THRUSTER_R]);
                pwmDevice[1].writeMicroseconds(pwmOut[THRUSTER_L]);
                Timer3.pwm(PIN_FR, pwmOut[THRUSTER_FR]);
                Timer3.pwm(PIN_FL, pwmOut[THRUSTER_FL]);
                Timer3.pwm(PIN_BL, pwmOut[THRUSTER_BL]);
                Timer3.pwm(PIN_BR, pwmOut[THRUSTER_BR]);
            }

            // ================================================================
            // Telemetry loop
            // ================================================================
            if (loopCount % COMM_LOOP_INTERVAL == 0) {
                sendTelemetry(nextRunTime);

                //eth.test();
                if (client.connected()){
                    Serial.println("client connected");
                    if(client.available()){
                        Serial.println("client available");
                        myByte = client.read();
                        if (myByte == 'a'){
                            pin = client.read() - 1;
                            val = client.read();
                            input_axes[pin] = val / 255.0;
                        } else if (myByte == 'd'){
                            pin = client.read() - 1;
                            val = client.read();
                            if (val == 'H'){
                                if (input_axes[pin] < 0){
                                    input_axes[pin] = input_axes[pin] * -1;
                                }
                            } else if (val == 'L'){
                                if (input_axes[pin] > 0){
                                    input_axes[pin] = input_axes[pin] * -1;
                                }
                            }
                        } else {
                          Serial.print("Unknown opcode: ");
                          Serial.println(char(myByte));
                        }
                    }
                }
            }

            loopCount++;
            loopCount = loopCount % 1000;
        }
    }

    return 0;
}
