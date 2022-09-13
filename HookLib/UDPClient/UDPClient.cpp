//
// Created by yll20 on 2022/8/18.
//
#define nil (0)
#define DESTINATION_IP "127.0.0.1"
#define DESTINATION_PORT 6000

#include "UDPClient.h"
SOCKET sockClient;
SOCKADDR_IN addrSrv;
void sendUdpPacked(const char* buffer){
    sendto(sockClient, buffer, (int)strlen(buffer), 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
}

int initUDPClient(){
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsadata;
    int err;
    err = WSAStartup(wVersionRequested, &wsadata);
    if (err!=nil){
        return -1;
    }
    if (LOBYTE(wsadata.wVersion)!=2 || HIBYTE(wsadata.wVersion)!=2){
        WSACleanup();
        return -1;
    }
    sockClient = socket(AF_INET, SOCK_DGRAM, 0);
    inet_pton(AF_INET, DESTINATION_IP, &addrSrv.sin_addr);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(DESTINATION_PORT);
    return 0;
}

void cleanUDPClient(){
    closesocket(sockClient);
    WSACleanup();
}