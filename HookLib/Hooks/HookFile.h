//
// Created by yll20 on 2022/9/14.
//

#ifndef HOOKDEMO_HOOKFILE_H
#define HOOKDEMO_HOOKFILE_H

#include "../UDPClient/UDPClient.h"
#include "../Utils/utils.h"
#include "../Locale/Locale.h"
#include "../Base64/base64.h"
#include "../MD5/md5.h"
#include "common.h"
#include <Windows.h>

#define _export extern "C" __declspec(dllexport)

// 文件操作 CreatFileA, WriteFile, ReadFile, CloseHandle
HANDLE (WINAPI *OldCreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileA;

BOOL (WINAPI *OldWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                            LPOVERLAPPED lpOverlapped) = WriteFile;

BOOL (WINAPI *OldReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                           LPOVERLAPPED lpOverlapped) = ReadFile;

BOOL (WINAPI *OldCloseHandle)(HANDLE hObject) = CloseHandle;

_export HANDLE
WINAPI NewCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                      DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    const std::string funcArgs[] = {"funcName", "lpFileName(base64)", "dwDesiredAccess", "dwShareMode",
                                    "lpSecurityAttributes", "dwCreationDisposition", "dwFlagsAndAttributes",
                                    "hTemplateFile", "result"};

    HANDLE result = OldCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
                                   dwCreationDisposition,
                                   dwFlagsAndAttributes, hTemplateFile);
// 判断文件名是否和自身相同，相同则加入(danger)
    std::string selfFileName = getSelfFileName();
    std::string fileName = getFilenameByPath(lpFileName);
    std::ostringstream outputStringBuilder;
    if (selfFileName == fileName) {
// 构建消息
        std::string base64FileName = base64_encode((const unsigned char *) lpFileName, strlen(lpFileName));
        std::string dangerdwDesiredAccess = std::to_string(dwDesiredAccess) + "(danger)";
        buildMessage(outputStringBuilder, funcArgs, "CreateFileA", base64FileName, dangerdwDesiredAccess, dwShareMode,
                     lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, result);
    } else {
// 构建消息
        std::string base64FileName = base64_encode((const unsigned char *) lpFileName, strlen(lpFileName));
        buildMessage(outputStringBuilder, funcArgs, "CreateFileA", base64FileName, dwDesiredAccess, dwShareMode,
                     lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, result);
    }
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export BOOL
WINAPI NewWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                    LPOVERLAPPED lpOverlapped) {
    const std::string funcArgs[] = {"funcName", "hFile", "lpBuffer(base64)", "nNumberOfBytesToWrite",
                                    "lpNumberOfBytesWritten", "lpOverlapped", "result"};

    BOOL result = OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
// 构建消息
    std::string lpBufferUtf8 = GbkToUtf8((const char *) lpBuffer);
    std::string base64Buffer = base64_encode((const unsigned char *) lpBufferUtf8.c_str(),
                                             strlen(lpBufferUtf8.c_str()));
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "WriteFile", hFile, base64Buffer, nNumberOfBytesToWrite,
                 lpNumberOfBytesWritten, lpOverlapped, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export BOOL
WINAPI NewReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                   LPOVERLAPPED lpOverlapped) {
    const std::string funcArgs[] = {"funcName", "hFile", "lpBuffer(base64)", "nNumberOfBytesToRead",
                                    "lpNumberOfBytesRead", "lpOverlapped", "result"};

    BOOL result = OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
// 将读取的内容进行md5后存入fileContentMap
    char *md5Result;
    md5Result = md5.digestMemory((BYTE *) lpBuffer, (int) nNumberOfBytesToRead);
// 存入格式: <md5, 1>
    fileContentMap[md5Result] = 1;
// 截断lpBuffer
    std::string lpBufferUtf8 = GbkToUtf8((const char *) lpBuffer);
    std::string base64Buffer = base64_encode((const unsigned char *) lpBufferUtf8.c_str(),
                                             strlen(lpBufferUtf8.c_str()));
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "ReadFile", hFile, base64Buffer, nNumberOfBytesToRead,
                 lpNumberOfBytesRead, lpOverlapped, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export BOOL WINAPI NewCloseHandle(HANDLE hObject) {
    const std::string funcArgs[] = {"funcName", "hObject", "result"};

    BOOL result = OldCloseHandle(hObject);
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "CloseHandle", hObject, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}


#endif //HOOKDEMO_HOOKFILE_H
