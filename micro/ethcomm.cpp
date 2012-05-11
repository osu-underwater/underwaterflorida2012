/*! \file ethcomm.cpp
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief Source for EthComm class.
 */

#include "ethcomm.h"

EthComm::EthComm() : server(23) {
    // Start up Ethernet services.
    Ethernet.begin(mac, ip, gateway, subnet);

    // Start the server.
    server.begin();
}

void EthComm::RX() {
    client = server.available();

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
            } else if (myByte == 'b'){
                buttons = client.read();
            } else {
              Serial.print("Unknown opcode: ");
              Serial.println(char(myByte));
            }
        }
    }
    else {
        client.connect();
    }
}

void EthComm::TX() {
    uint8_t send_bytes[MAX_SEND_LENGTH];
    client = server.available();

    if (client.connected()){
        client.flush();
        send_bytes[0] = 'D';
        for (int j = 0; j < 3; j++){
            for (int i = 0; i < 3; i++){
                send_bytes[j*3 + i + 1] = bodyDCM[j][i] * 127 + 127;
            }
        }
        client.write(send_bytes, 10);
        send_bytes[0] = 'P';
        send_bytes[1] = (uint8_t)PID[PID_ANG_POS_X].P;
        send_bytes[2] = (uint8_t)PID[PID_ANG_VEL_X].P;
        send_bytes[3] = (uint8_t)(PID[PID_ANG_VEL_X].D * -100.0);
        client.write(send_bytes, 4);
    } else {
        client.connect();
    }
}

