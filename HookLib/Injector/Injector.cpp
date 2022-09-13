//
// Created by yll20 on 2022/8/19.
//

#include <Windows.h>
#include <iostream>
#include "detours.h"

int main(int argc, char* argv[]) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    char *DirPath = new char[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, DirPath);
    char *DllPath = new char[MAX_PATH];
    // dll path = current directory + dll name
    strcpy(DllPath, DirPath);

    strcat(DllPath, "\\HookDemo.dll");
    char *TargetExePath = new char[MAX_PATH];
    // 第一个参数
    strcpy(TargetExePath, argv[1]);

    if (DetourCreateProcessWithDllEx(TargetExePath, nullptr, nullptr, nullptr, TRUE,
                                     CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, nullptr,
                                     DirPath, &si, &pi, DllPath, nullptr)) {
        ResumeThread(pi.hThread);
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    else{
        printf("Error: %lu\n", GetLastError());
    }
    return 0;
}