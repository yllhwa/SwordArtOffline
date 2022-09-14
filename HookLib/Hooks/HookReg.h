//
// Created by yll20 on 2022/9/14.
//
#ifndef HOOKDEMO_HOOKREG_H
#define HOOKDEMO_HOOKREG_H

#include "common.h"
#include "../UDPClient/UDPClient.h"
#include "../Utils/utils.h"
#include "../Locale/Locale.h"
#include "../Base64/base64.h"
#include "../MD5/md5.h"
#include <Windows.h>

#define _export extern "C" __declspec(dllexport)

// 注册表操作 RegCreateKeyEx, RegOpenKeyEx, RegSetValueEx, RegCloseKey, RegDeleteKey, RegDeleteValue
static LSTATUS (WINAPI *OldRegCreateKeyEx)(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions,
                                           REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                           PHKEY phkResult, LPDWORD lpdwDisposition) = RegCreateKeyEx;

static LSTATUS (WINAPI *OldRegOpenKeyEx)(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired,
                                         PHKEY phkResult) = RegOpenKeyEx;

static LSTATUS (WINAPI *OldRegSetValueEx)(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType,
                                          const BYTE *lpData, DWORD cbData) = RegSetValueEx;

static LSTATUS (WINAPI *OldRegCloseKey)(HKEY hKey) = RegCloseKey;

static LSTATUS (WINAPI *OldRegDeleteKey)(HKEY hKey, LPCWSTR lpSubKey) = RegDeleteKey;

static LSTATUS (WINAPI *OldRegDeleteValue)(HKEY hKey, LPCWSTR lpValueName) = RegDeleteValue;

// 注册表操作实现

_export LONG
WINAPI NewRegCreateKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions,
                         REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                         PHKEY phkResult, LPDWORD lpdwDisposition) {
    const std::string funcArgs[] = {"funcName", "hKey", "lpSubKey(base64)", "Reserved", "lpClass", "dwOptions",
                                    "samDesired",
                                    "lpSecurityAttributes", "phkResult", "lpdwDisposition", "result"};

    LONG result = OldRegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes,
                                    phkResult, lpdwDisposition);

    std::string lpSubKeyUtf8 = wstring_to_utf8(lpSubKey);
    std::string lpSubKeyBase64 = base64_encode((const unsigned char *) lpSubKeyUtf8.c_str(),
                                               strlen(lpSubKeyUtf8.c_str()));
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "RegCreateKeyEx", hKey, lpSubKeyBase64, Reserved, lpClass, dwOptions,
                 samDesired, lpSecurityAttributes, phkResult, lpdwDisposition, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export LONG
WINAPI NewRegOpenKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired,
                       PHKEY phkResult) {
    const std::string funcArgs[] = {"funcName", "hKey", "lpSubKey(base64)", "ulOptions", "samDesired", "phkResult",
                                    "result"};

    LONG result = OldRegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    std::string lpSubKeyUtf8 = wstring_to_utf8(lpSubKey);
    std::string lpSubKeyBase64 = base64_encode((const unsigned char *) lpSubKeyUtf8.c_str(),
                                               strlen(lpSubKeyUtf8.c_str()));
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "RegOpenKeyEx", hKey, lpSubKeyBase64, ulOptions, samDesired, phkResult,
                 result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export LONG
WINAPI NewRegSetValueEx(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType,
                        const BYTE *lpData, DWORD cbData) {
    const std::string funcArgs[] = {"funcName", "hKey", "lpValueName(base64)", "Reserved", "dwType", "lpData(base64)",
                                    "cbData", "result"};
    LONG result = OldRegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);
    std::string lpValueNameUtf8 = wstring_to_utf8(lpValueName);
    std::string lpValueNameBase64 = base64_encode((const unsigned char *) lpValueNameUtf8.c_str(),
                                                  strlen(lpValueNameUtf8.c_str()));
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "RegSetValueEx", hKey, lpValueNameBase64, Reserved, dwType,
                 base64_encode(lpData, cbData), cbData, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export LONG WINAPI NewRegCloseKey(HKEY hKey) {
    const std::string funcArgs[] = {"funcName", "hKey", "result"};

    LONG result = OldRegCloseKey(hKey);
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "RegCloseKey", hKey, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export LONG WINAPI NewRegDeleteKey(HKEY hKey, LPCWSTR lpSubKey) {
    const std::string funcArgs[] = {"funcName", "hKey", "lpSubKey(base64)", "result"};

    LONG result = OldRegDeleteKey(hKey, lpSubKey);
    std::string lpSubKeyUtf8 = wstring_to_utf8(lpSubKey);
    std::string lpSubKeyBase64 = base64_encode((const unsigned char *) lpSubKeyUtf8.c_str(),
                                               strlen(lpSubKeyUtf8.c_str()));
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "RegDeleteKey", hKey, lpSubKeyBase64, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
_export LONG WINAPI NewRegDeleteValue(HKEY hKey, LPCWSTR lpValueName) {
    const std::string funcArgs[] = {"funcName", "hKey", "lpValueName(base64)", "result"};

    LONG result = OldRegDeleteValue(hKey, lpValueName);
    std::string lpValueNameUtf8 = wstring_to_utf8(lpValueName);
    std::string lpValueNameBase64 = base64_encode((const unsigned char *) lpValueNameUtf8.c_str(),
                                                  strlen(lpValueNameUtf8.c_str()));
    std::ostringstream outputStringBuilder;
    buildMessage(outputStringBuilder, funcArgs, "RegDeleteValue", hKey, lpValueNameBase64, result);
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}


#endif //HOOKDEMO_HOOKREG_H
