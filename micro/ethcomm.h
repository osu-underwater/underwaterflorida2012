/*! \file ethcomm.h
 *  \author Soo-Hyun Yoo
 *  \brief Header for EthComm class.
 */

#ifndef ETHCOMM_H
#define ETHCOMM_H

#include <SPI.h>
#include <Ethernet.h>

class EthComm {
    // Network information.
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t gateway[4];
    uint8_t subnet[4];

    EthernetServer server;
    EthernetClient client;

public:
    EthComm();
};

#endif // ETHCOMM_H

