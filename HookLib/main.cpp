#define UNICODE

#include "UDPClient/UDPClient.h"
#include "detours/detours.h"
#include "LuaEngine/LuaEngine.h"
#include "Hooks/common.h"
#include "Hooks/HookMessageBox.h"
#include "Hooks/HookFile.h"
#include "Hooks/HookHeap.h"
#include "Hooks/HookSocket.h"
#include "Hooks/HookReg.h"
#include "Hooks/HookMem.h"

#define ATTACH(funcName) DetourAttach(&(PVOID &) Old##funcName, New##funcName)
#define DETACH(funcName) DetourDetach(&(PVOID &) Old##funcName, New##funcName)

void StartHook() {
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // 弹窗
    ATTACH(MessageBoxA);
    ATTACH(MessageBoxW);
    // 文件操作
    ATTACH(CreateFileA);
    ATTACH(WriteFile);
    ATTACH(ReadFile);
    // ATTACH(CloseHandle);
    // 堆操作
    ATTACH(HeapCreate);
    ATTACH(HeapAlloc);
    ATTACH(HeapFree);
    ATTACH(HeapDestroy);
    // 注册表操作
    ATTACH(RegCreateKeyEx);
    ATTACH(RegOpenKeyEx);
    ATTACH(RegSetValueEx);
    ATTACH(RegCloseKey);
    ATTACH(RegDeleteKey);
    ATTACH(RegDeleteValue);
    // socket操作
    ATTACH(Socket);
    ATTACH(Bind);
    ATTACH(Connect);
    ATTACH(Send);
    ATTACH(Recv);
    ATTACH(Close);
    // 内存操作
    ATTACH(Memcpy);
    DetourTransactionCommit();
}

//解除钩子
void EndHook() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // 弹窗
    DETACH(MessageBoxA);
    DETACH(MessageBoxW);
    // 文件操作
    DETACH(CreateFileA);
    DETACH(WriteFile);
    DETACH(ReadFile);
    // DETACH(CloseHandle);
    // 堆操作
    DETACH(HeapCreate);
    DETACH(HeapAlloc);
    DETACH(HeapFree);
    DETACH(HeapDestroy);
    // 注册表操作
    DETACH(RegCreateKeyEx);
    DETACH(RegOpenKeyEx);
    DETACH(RegSetValueEx);
    DETACH(RegCloseKey);
    DETACH(RegDeleteKey);
    DETACH(RegDeleteValue);
    // socket操作
    DETACH(Socket);
    DETACH(Bind);
    DETACH(Connect);
    DETACH(Send);
    DETACH(Recv);
    DETACH(Close);
    // 内存操作
    DETACH(Memcpy);
    DetourTransactionCommit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            //避免被重复加载
            initLocks();
            initUDPClient();
            initLuaEngine();
            StartHook();
            break;
        case DLL_THREAD_ATTACH:
            initLocks();
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            EndHook();
            break;
        default:
            break;
    }
    return TRUE;
}
