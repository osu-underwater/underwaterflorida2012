/*! \file micro.ino
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief ROV main controller loop.
 */

#include "ethcomm.cpp"

int main(void) {
    init();   // For Arduino.

    Serial.begin(115200);

    //EthComm eth;

    Serial.println("Setup complete.");

    while (1) {
        //eth.test();
    }

    return 0;
}

