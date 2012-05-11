/*! \file ethcomm.cpp
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief Source for EthComm class.
 */

#include "ethcomm.h"

EthComm::EthComm() : server(23) {
    server.begin();
    client = server.available();

    Ethernet.begin(mac, ip, gateway, subnet);
}

void EthComm::test() {
    if (client.connected()){
        if(client.available()){
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

