//
// Created by yll20 on 2022/9/14.
//

#ifndef HOOKDEMO_HOOKHEAP_H
#define HOOKDEMO_HOOKHEAP_H

#include "common.h"
#include "../UDPClient/UDPClient.h"
#include "../Utils/utils.h"
#include "../Locale/Locale.h"
#include "../Base64/base64.h"
#include <unordered_set>
#include <Windows.h>

#define _export extern "C" __declspec(dllexport)

// 堆操作 HeapCreate, HeapAlloc, HeapFree, HeapDestroy
HANDLE (WINAPI *OldHeapCreate)(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) = HeapCreate;

LPVOID (WINAPI *OldHeapAlloc)(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) = HeapAlloc;

BOOL (WINAPI *OldHeapFree)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) = HeapFree;

BOOL (WINAPI *OldHeapDestroy)(HANDLE hHeap) = HeapDestroy;

// heap操作实现

std::unordered_set<int> heapHandleSet;

_export HANDLE
WINAPI NewHeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    const std::string funcArgs[] = {"funcName", "flOptions", "dwInitialSize", "dwMaximumSize", "result"};

    HANDLE result = OldHeapCreate(flOptions, dwInitialSize, dwMaximumSize);
    LONG _funcLock = (LONG) TlsGetValue(heapCreateLock);
    if (_funcLock == 0) {
        TlsSetValue(heapCreateLock, (LPVOID) 1);
        heapHandleSet.insert((int) result);
// send udp message
        std::ostringstream outputStringBuilder;
        buildMessage(outputStringBuilder, funcArgs, "HeapCreate", flOptions, dwInitialSize, dwMaximumSize, result);
        sendUdpPacked(outputStringBuilder.str().c_str());
        TlsSetValue(heapCreateLock, (LPVOID) 0);
    }
    return result;
}

std::unordered_set<int> heapSet;


_export LPVOID WINAPI NewHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) {
    const std::string funcArgs[] = {"funcName", "hHeap", "dwFlags", "dwBytes", "result"};

    LPVOID result = OldHeapAlloc(hHeap, dwFlags, dwBytes);
    LONG _funcLock = (LONG) TlsGetValue(heapAllocLock);
    if (_funcLock == 0) {
        TlsSetValue(heapAllocLock, (LPVOID) 1);
        heapSet.insert((int) result);
        TlsSetValue(heapAllocLock, (LPVOID) 0);
    }
    return result;
}

_export BOOL WINAPI NewHeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    const std::string funcArgs[] = {"funcName", "hHeap", "dwFlags", "lpMem", "result"};

    BOOL result = OldHeapFree(hHeap, dwFlags, lpMem);
// 若hHeap未被创建，则不记录
    if (heapHandleSet.find((int) hHeap) == heapHandleSet.end()) {
        return result;
    }
    LONG _funcLock = (LONG) TlsGetValue(heapFreeLock);
    if (_funcLock == 0) {
        TlsSetValue(heapFreeLock, (LPVOID) 1);
        if (heapSet.find((int) lpMem) == heapSet.end()) {
            std::ostringstream outputStringBuilder;
            buildMessage(outputStringBuilder, funcArgs, "HeapFree", hHeap, dwFlags, lpMem, result);
            sendUdpPacked(outputStringBuilder.str().c_str());
        } else {
            heapSet.erase((int) lpMem);
        }
        TlsSetValue(heapFreeLock, (LPVOID) 0);
    }
    return result;
}
_export BOOL WINAPI NewHeapDestroy(HANDLE hHeap) {
    const std::string funcArgs[] = {"funcName", "hHeap", "result", "extra"};

    BOOL result = OldHeapDestroy(hHeap);
    LONG _funcLock = (LONG) TlsGetValue(heapDestroyLock);
    if (_funcLock == 0) {
        TlsSetValue(heapDestroyLock, (LPVOID) 1);
        if (heapHandleSet.find((int) hHeap) == heapHandleSet.end()) {
            std::ostringstream outputStringBuilder;
            std::string extraMessage = "UntrackedHeapHandle";
            buildMessage(outputStringBuilder, funcArgs, "HeapDestroy", hHeap, result, extraMessage);
            sendUdpPacked(outputStringBuilder.str().c_str());
        } else {
            std::ostringstream outputStringBuilder;
            buildMessage(outputStringBuilder, funcArgs, "HeapDestroy", hHeap, result);
            sendUdpPacked(outputStringBuilder.str().c_str());
            heapHandleSet.erase((int) hHeap);
        }
        TlsSetValue(heapDestroyLock, (LPVOID) 0);
    }
    return result;
}

#endif //HOOKDEMO_HOOKHEAP_H
