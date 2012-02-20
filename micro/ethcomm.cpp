/*! \file ethcomm.cpp
 *  \author Nathan Murrow, Soo-Hyun Yoo
 *  \brief Source for EthComm class.
 */

#include "ethcomm.h"

EthComm::EthComm() : server(23) {
    mac     = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed};
    ip      = {192,169,0,177};
    gateway = {192,169,0,1};
    subnet  = {255,255,255,0};

    server.begin();
    client = server.available();

    Ethernet.begin(mac, ip, gateway, subnet);
}

