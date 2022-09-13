//
// Created by yll20 on 2022/8/18.
//

#ifndef HOOKDEMO_UDPCLIENT_H
#define HOOKDEMO_UDPCLIENT_H
#include <Winsock2.h>
#include <WS2tcpip.h>

void sendUdpPacked(const char* buffer);
int initUDPClient();
void cleanUDPClient();

#endif //HOOKDEMO_UDPCLIENT_H
