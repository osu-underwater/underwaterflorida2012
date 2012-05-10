/*! \file ethcomm.h
 *  \author Soo-Hyun Yoo
 *  \brief Header for EthComm class.
 */

#ifndef ETHCOMM_H
#define ETHCOMM_H

#include <SPI.h>
#include <Ethernet.h>
#include "globals.h"

class EthComm {
    int pin, val;
    uint8_t myByte;

    Server server;
    Client client;

public:
    EthComm();
    void test();
};

#endif // ETHCOMM_H

