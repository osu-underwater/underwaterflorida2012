/*! \file ethcomm.h
 *  \author Soo-Hyun Yoo
 *  \brief Header for EthComm class.
 */

#ifndef ETHCOMM_H
#define ETHCOMM_H

#include "globals.h"

uint8_t mac[]     = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed};
uint8_t ip[]      = {192,169,0,177};
uint8_t gateway[] = {192,169,0,1};
uint8_t subnet[]  = {255,255,255,0};

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

