/*! \file micro.ino
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief ROV main controller loop.
 */

#include "ethcomm.h"

int main(void) {
    init();   // For Arduino.

    Serial.begin(115200);


    int pin, val;

    Serial.println("Setup complete.");

    uint8_t mybyte;

    while (1) {
        if (client.connected()){
            if(client.available()){
                mybyte = client.read();
                if (mybyte == 'a'){
                    pin = client.read();
                    val = client.read();
                    analogWrite(pin, val);
                } else if (mybyte == 'd'){
                    pin = client.read();
                    val = client.read();
                    if (val == 'H'){
                        digitalWrite(pin, HIGH);
                    } else if (val == 'L'){
                        digitalWrite(pin, LOW);
                    }
                } else {
                  Serial.print("Unknown opcode: ");
                  Serial.println(char(mybyte));
                }
            }
        }
    }

    return 0;
}

