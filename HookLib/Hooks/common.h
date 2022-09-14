//
// Created by yll20 on 2022/9/14.
//
#define UNICODE
#ifndef HOOKDEMO_COMMON_H
#define HOOKDEMO_COMMON_H

#include <map>
#include <string>
#include "../MD5/md5.h"

std::map<std::string, int> fileContentMap;

static LONG heapCreateLock = 0;
static LONG heapAllocLock = 0;
static LONG heapFreeLock = 0;
static LONG heapDestroyLock = 0;

MD5 md5;

void initLocks() {
    heapCreateLock = TlsAlloc();
    heapAllocLock = TlsAlloc();
    heapFreeLock = TlsAlloc();
    heapDestroyLock = TlsAlloc();
    TlsSetValue(heapCreateLock, 0);
    TlsSetValue(heapAllocLock, (PVOID) 0);
    TlsSetValue(heapFreeLock, (PVOID) 0);
    TlsSetValue(heapDestroyLock, (PVOID) 0);
}

#endif //HOOKDEMO_COMMON_H
