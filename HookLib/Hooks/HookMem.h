//
// Created by yll20 on 2022/9/16.
//

#ifndef HOOKDEMO_HOOKMEM_H
#define HOOKDEMO_HOOKMEM_H

#include "common.h"
#include "../UDPClient/UDPClient.h"
#include "../Utils/utils.h"
#include "../Locale/Locale.h"
#include "../Base64/base64.h"
#define _export extern "C" __declspec(dllexport)


void* (*OldMemcpy)(void *dest, const void *src, size_t count) = memcpy;

int enableTracingMemcpy = 1;
_export void* NewMemcpy(void *dest, const void *src, size_t count) {
    const std::string funcArgs[] = {"funcName", "dest", "src", "count", "result"};

    void* result = OldMemcpy(dest, src, count);
    if (enableTracingMemcpy == 1) {
        enableTracingMemcpy = 0;
        std::ostringstream outputStringBuilder;
        buildMessage(outputStringBuilder, funcArgs, "memcpy", dest, src, count, result);
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingMemcpy = 1;
    }
    return result;
}

#endif //HOOKDEMO_HOOKMEM_H
