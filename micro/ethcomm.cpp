/*! \file ethcomm.cpp
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief Source for EthComm class.
 */

#include "ethcomm.h"

EthComm::EthComm() : server(23) {
    uint8_t mac[]     = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed};
    uint8_t ip[]      = {192,169,0,177};
    uint8_t gateway[] = {192,169,0,1};
    uint8_t subnet[]  = {255,255,255,0};

    server.begin();
    client = server.available();

    Ethernet.begin(mac, ip, gateway, subnet);
}

void EthComm::test() {
    if (client.connected()){
        if(client.available()){
            myByte = client.read();
            if (myByte == 'a'){
                pin = client.read();
                val = client.read();
                analogWrite(pin, val);
            } else if (myByte == 'd'){
                pin = client.read();
                val = client.read();
                if (val == 'H'){
                    digitalWrite(pin, HIGH);
                } else if (val == 'L'){
                    digitalWrite(pin, LOW);
                }
            } else {
              Serial.print("Unknown opcode: ");
              Serial.println(char(myByte));
            }
        }
    }
}

