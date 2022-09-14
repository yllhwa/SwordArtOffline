//
// Created by yll20 on 2022/9/14.
//

#ifndef HOOKDEMO_HOOKSOCKET_H
#define HOOKDEMO_HOOKSOCKET_H

#include "common.h"
#include "../UDPClient/UDPClient.h"
#include "../Utils/utils.h"
#include "../Locale/Locale.h"
#include "../Base64/base64.h"
#include "../MD5/md5.h"
#include <Windows.h>

#define _export extern "C" __declspec(dllexport)

// socket操作 socket、bind、connect、send、recv、close
SOCKET (WINAPI *OldSocket)(int af, int type, int protocol) = socket;

static int (WINAPI *OldBind)(SOCKET s, const sockaddr *name, int namelen) = bind;

static int (WINAPI *OldConnect)(SOCKET s, const sockaddr *name, int namelen) = connect;

static int (WINAPI *OldSend)(SOCKET s, const char *buf, int len, int flags) = send;

static int (WINAPI *OldRecv)(SOCKET s, char *buf, int len, int flags) = recv;

static int (WINAPI *OldClose)(SOCKET s) = closesocket;


// socket操作实现

int enableTracingSocket = 1;
_export SOCKET WINAPI NewSocket(int af, int type, int protocol) {
    const std::string funcArgs[] = {"funcName", "af", "type", "protocol", "result"};

    SOCKET result = OldSocket(af, type, protocol);
    if (enableTracingSocket == 1) {
        enableTracingSocket = 0;
        std::ostringstream outputStringBuilder;
        buildMessage(outputStringBuilder, funcArgs, "socket", af, type, protocol, result);
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingSocket = 1;
    }
    return result;
}

int enableTracingBind = 1;
_export int WINAPI NewBind(SOCKET s, const sockaddr *name, int namelen) {
    const std::string funcArgs[] = {"funcName", "s", "name", "namelen", "result"};

    int result = OldBind(s, name, namelen);
    if (enableTracingBind == 1) {
        enableTracingBind = 0;
        std::ostringstream outputStringBuilder;
        buildMessage(outputStringBuilder, funcArgs, "bind", s, name, namelen, result);
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingBind = 1;
    }
    return result;
}

std::string sockaddrToString(const sockaddr *name) {
    std::string result;
    if (name->sa_family == AF_INET) {
        auto *addr = (sockaddr_in *) name;
        result = inet_ntoa(addr->sin_addr);
    } else if (name->sa_family == AF_INET6) {
        auto *addr = (sockaddr_in6 *) name;
        char ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &addr->sin6_addr, ip, INET6_ADDRSTRLEN);
        result = ip;
    }
    // 端口
    result += ":";
    result += std::to_string(ntohs(((sockaddr_in *) name)->sin_port));
    return result;
}

int enableTracingConnect = 1;
_export int WINAPI NewConnect(SOCKET s, const sockaddr *name, int namelen) {
    const std::string funcArgs[] = {"funcName", "s", "name", "namelen", "result"};

    int result = OldConnect(s, name, namelen);
    if (enableTracingConnect == 1) {
        enableTracingConnect = 0;
        std::ostringstream outputStringBuilder;
        // 将sockaddr转化为目的地址
        std::string destAddr = sockaddrToString(name);
        buildMessage(outputStringBuilder, funcArgs, "connect", s, destAddr, namelen, result);
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingConnect = 1;
    }
    return result;
}

int enableTracingSend = 1;
_export int WINAPI NewSend(SOCKET s, const char *buf, int len, int flags) {
    const std::string funcArgs[] = {"funcName", "s", "buf(base64)", "len", "flags", "result"};

    int result = OldSend(s, buf, len, flags);
    if (enableTracingSend == 1) {
        enableTracingSend = 0;
        // 检查是否是从文件读取的数据(fileContentMap)
        char *bufMd5 = md5.digestMemory((BYTE *) buf, len);
        std::string bufMd5Str = bufMd5;
        // 从fileContentMap中查找
        auto iter = fileContentMap.find(bufMd5Str);
        if (iter != fileContentMap.end()) {
            std::string bufBase64 = base64_encode((const unsigned char *) buf, len);
            std::string sDanger = std::to_string(s) + "(danger)";
            std::ostringstream outputStringBuilder;
            buildMessage(outputStringBuilder, funcArgs, "send", sDanger, bufBase64, len, flags, result);
            sendUdpPacked(outputStringBuilder.str().c_str());
        } else {
            std::string bufBase64 = base64_encode((const unsigned char *) buf, len);
            std::ostringstream outputStringBuilder;
            buildMessage(outputStringBuilder, funcArgs, "send", s, bufBase64, len, flags, result);
            sendUdpPacked(outputStringBuilder.str().c_str());
        }
        enableTracingSend = 1;
    }
    return result;
}

int enableTracingRecv = 1;
_export int WINAPI NewRecv(SOCKET s, char *buf, int len, int flags) {
    const std::string funcArgs[] = {"funcName", "s", "buf(base64)", "len", "flags", "result"};

    int result = OldRecv(s, buf, len, flags);
    if (enableTracingRecv == 1) {
        enableTracingRecv = 0;
        std::string bufBase64 = base64_encode((const unsigned char *) buf, result);
        std::ostringstream outputStringBuilder;
        buildMessage(outputStringBuilder, funcArgs, "recv", s, bufBase64, len, flags, result);
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingRecv = 1;
    }
    return result;
}

int enableTracingClose = 1;
_export int WINAPI NewClose(SOCKET s) {
    const std::string funcArgs[] = {"funcName", "s", "result"};

    int result = OldClose(s);
    if (enableTracingClose == 1) {
        enableTracingClose = 0;
        std::ostringstream outputStringBuilder;
        buildMessage(outputStringBuilder, funcArgs, "close", s, result);
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingClose = 1;
    }
    return result;
}

#endif //HOOKDEMO_HOOKSOCKET_H
