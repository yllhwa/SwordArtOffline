#define UNICODE

#include <sstream>
#include <string>
#include <unordered_set>
#include "UDPClient/UDPClient.h"
#include "Base64/base64.h"
#include "detours.h"
#include "Locale/Locale.h"

static LONG heapCreateLock = 0;
static LONG heapAllocLock = 0;
static LONG heapFreeLock = 0;
static LONG heapDestroyLock = 0;

void StartHook();

void EndHook();

// 弹窗 MessageBoxA、MessageBoxW
static int (WINAPI *OldMessageBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) = MessageBoxA;

static int (WINAPI *OldMessageBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) = MessageBoxW;

// 弹窗实现
extern "C" __declspec(dllexport) int WINAPI NewMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    int result = OldMessageBoxA(hWnd, lpText, lpCaption, uType);
    // lpText和lpCation为gbk编码，需要转换为utf8编码
    std::string strText = GbkToUtf8(lpText);
    std::string strCaption = GbkToUtf8(lpCaption);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "MessageBoxA"
                        << "\nhWnd\n" << hWnd
                        << "\nlpText(base64)\n"
                        << base64_encode((unsigned char const *) strText.c_str(), strlen(strText.c_str()))
                        << "\nlpCaption(base64)\n"
                        << base64_encode((unsigned char const *) strCaption.c_str(), strlen(strCaption.c_str()))
                        << "\nuType\n" << uType
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}

extern "C" __declspec(dllexport) int WINAPI NewMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    int result = OldMessageBoxW(hWnd, lpText, lpCaption, uType);
    std::ostringstream outputStringBuilder;
    // lpText和lpCaption都是Unicode字符串，转为UTF-8字符串
    std::string lpTextUtf8 = wstring_to_utf8(lpText);
    std::string lpCaptionUtf8 = wstring_to_utf8(lpCaption);
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "MessageBoxW"
                        << "\nhWnd\n" << hWnd
                        << "\nlpText(base64)\n"
                        //                        << base64_encode((unsigned char const *) lpText, wcslen(lpText) * sizeof(wchar_t))
                        << base64_encode((unsigned char const *) lpTextUtf8.c_str(), strlen(lpTextUtf8.c_str()))
                        << "\nlpCaption(base64)\n"
                        //                        << base64_encode((unsigned char const *) lpCaption, wcslen(lpCaption) * sizeof(wchar_t))
                        << base64_encode((unsigned char const *) lpCaptionUtf8.c_str(), strlen(lpCaptionUtf8.c_str()))
                        << "\nuType\n" << uType
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}

// 文件操作 CreatFileA, WriteFile, ReadFile, CloseHandle
HANDLE (WINAPI *OldCreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileA;

BOOL (WINAPI *OldWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                            LPOVERLAPPED lpOverlapped) = WriteFile;

BOOL (WINAPI *OldReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                           LPOVERLAPPED lpOverlapped) = ReadFile;

BOOL (WINAPI *OldCloseHandle)(HANDLE hObject) = CloseHandle;

extern "C" __declspec(dllexport) HANDLE
WINAPI NewCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                      LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                      DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE result = OldCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                                   dwFlagsAndAttributes, hTemplateFile);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "CreateFileA"
                        << "\nlpFileName(base64)\n"
                        << base64_encode((unsigned char const *) lpFileName, strlen(lpFileName))
                        << "\ndwDesiredAccess\n" << dwDesiredAccess
                        << "\ndwShareMode\n" << dwShareMode
                        << "\nlpSecurityAttributes\n" << lpSecurityAttributes
                        << "\ndwCreationDisposition\n" << dwCreationDisposition
                        << "\ndwFlagsAndAttributes\n" << dwFlagsAndAttributes
                        << "\nhTemplateFile\n" << hTemplateFile
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) BOOL
WINAPI NewWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
                    LPOVERLAPPED lpOverlapped) {
    BOOL result = OldWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
    std::string lpBufferUtf8 = GbkToUtf8((const char *) lpBuffer);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "WriteFile"
                        << "\nhFile\n" << hFile
                        << "\nlpBuffer(base64)\n"
                        << base64_encode((unsigned char const *) lpBufferUtf8.c_str(), strlen(lpBufferUtf8.c_str()))
                        << "\nnNumberOfBytesToWrite\n" << nNumberOfBytesToWrite
                        << "\nlpNumberOfBytesWritten\n" << lpNumberOfBytesWritten
                        << "\nlpOverlapped\n" << lpOverlapped
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) BOOL
WINAPI NewReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
                   LPOVERLAPPED lpOverlapped) {
    BOOL result = OldReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    // 截断lpBuffer
    std::string lpBufferUtf8 = GbkToUtf8((const char *) lpBuffer);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "ReadFile"
                        << "\nhFile\n" << hFile
                        << "\nlpBuffer(base64)\n"
                        << base64_encode((unsigned char const *) lpBufferUtf8.c_str(), strlen(lpBufferUtf8.c_str()))
                        << "\nnNumberOfBytesToRead\n" << nNumberOfBytesToRead
                        << "\nlpNumberOfBytesRead\n" << lpNumberOfBytesRead
                        << "\nlpOverlapped\n" << lpOverlapped
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) BOOL WINAPI NewCloseHandle(HANDLE hObject) {
    BOOL result = OldCloseHandle(hObject);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "CloseHandle"
                        << "\nhObject\n" << hObject
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}

// 堆操作 HeapCreate, HeapAlloc, HeapFree, HeapDestroy
HANDLE (WINAPI *OldHeapCreate)(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) = HeapCreate;

LPVOID (WINAPI *OldHeapAlloc)(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) = HeapAlloc;

BOOL (WINAPI *OldHeapFree)(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) = HeapFree;

BOOL (WINAPI *OldHeapDestroy)(HANDLE hHeap) = HeapDestroy;

std::unordered_set<int> heapHandleSet;

extern "C" __declspec(dllexport) HANDLE
WINAPI NewHeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize) {
    HANDLE result = OldHeapCreate(flOptions, dwInitialSize, dwMaximumSize);
    LONG _funcLock = (LONG) TlsGetValue(heapCreateLock);
    if (_funcLock == 0) {
        TlsSetValue(heapCreateLock, (LPVOID) 1);
        heapHandleSet.insert((int) result);
        // send udp message
        std::ostringstream outputStringBuilder;
        outputStringBuilder << "pid\n" << GetCurrentProcessId()
                            << "\nfuncName\n" << "HeapCreate"
                            << "\nflOptions\n" << flOptions
                            << "\ndwInitialSize\n" << dwInitialSize
                            << "\ndwMaximumSize\n" << dwMaximumSize
                            << "\nresult\n" << result;
        sendUdpPacked(outputStringBuilder.str().c_str());
        TlsSetValue(heapCreateLock, (LPVOID) 0);
    }
    return result;
}

std::unordered_set<int> heapSet;


extern "C" __declspec(dllexport) LPVOID WINAPI NewHeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes) {

    LPVOID result = OldHeapAlloc(hHeap, dwFlags, dwBytes);
    LONG _funcLock = (LONG) TlsGetValue(heapAllocLock);
    if (_funcLock == 0) {
        TlsSetValue(heapAllocLock, (LPVOID) 1);
        heapSet.insert((int) result);
        TlsSetValue(heapAllocLock, (LPVOID) 0);
    }
    return result;
}

extern "C" __declspec(dllexport) BOOL WINAPI NewHeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    BOOL result = OldHeapFree(hHeap, dwFlags, lpMem);
    LONG _funcLock = (LONG) TlsGetValue(heapFreeLock);
    if (_funcLock == 0) {
        TlsSetValue(heapFreeLock, (LPVOID) 1);
        if (heapSet.find((int) lpMem) == heapSet.end()) {
            std::ostringstream outputStringBuilder;
            outputStringBuilder << "pid\n" << GetCurrentProcessId()
                                << "\nfuncName\n" << "HeapFree"
                                << "\nhHeap\n" << hHeap
                                << "\ndwFlags\n" << dwFlags
                                << "\nlpMem\n" << lpMem
                                << "\nresult\n" << result;
            sendUdpPacked(outputStringBuilder.str().c_str());
        } else {
            heapSet.erase((int) lpMem);
        }
        TlsSetValue(heapFreeLock, (LPVOID) 0);
    }
    return result;
}
extern "C" __declspec(dllexport) BOOL WINAPI NewHeapDestroy(HANDLE hHeap) {
    BOOL result = OldHeapDestroy(hHeap);
    LONG _funcLock = (LONG) TlsGetValue(heapDestroyLock);
    if (_funcLock == 0) {
        TlsSetValue(heapDestroyLock, (LPVOID) 1);
        if (heapHandleSet.find((int) hHeap) == heapHandleSet.end()) {
            std::ostringstream outputStringBuilder;
            outputStringBuilder << "pid\n" << GetCurrentProcessId()
                                << "\nfuncName\n" << "HeapDestroy"
                                << "\nhHeap\n" << hHeap << "(danger)"
                                << "\nresult\n" << result;
            sendUdpPacked(outputStringBuilder.str().c_str());
        } else {
            std::ostringstream outputStringBuilder;
            outputStringBuilder << "pid\n" << GetCurrentProcessId()
                                << "\nfuncName\n" << "HeapDestroy"
                                << "\nhHeap\n" << hHeap
                                << "\nresult\n" << result;
            sendUdpPacked(outputStringBuilder.str().c_str());
            heapHandleSet.erase((int) hHeap);
        }
        TlsSetValue(heapDestroyLock, (LPVOID) 0);
    }
    return result;
}

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

extern "C" __declspec(dllexport) LONG
WINAPI NewRegCreateKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions,
                         REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                         PHKEY phkResult, LPDWORD lpdwDisposition) {
    LONG result = OldRegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes,
                                    phkResult, lpdwDisposition);
    std::string lpSubKeyUtf8 = wstring_to_utf8(lpSubKey);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "RegCreateKeyEx"
                        << "\nhKey\n" << hKey
                        << "\nlpSubKey(base64)\n"
                        << base64_encode((unsigned const char *) lpSubKeyUtf8.c_str(), strlen(lpSubKeyUtf8.c_str()))
                        << "\nReserved\n" << Reserved
                        << "\nlpClass\n" << lpClass
                        << "\ndwOptions\n" << dwOptions
                        << "\nsamDesired\n" << samDesired
                        << "\nlpSecurityAttributes\n" << lpSecurityAttributes
                        << "\nphkResult\n" << phkResult
                        << "\nlpdwDisposition\n" << lpdwDisposition
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) LONG
WINAPI NewRegOpenKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired,
                       PHKEY phkResult) {
    LONG result = OldRegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    std::string lpSubKeyUtf8 = wstring_to_utf8(lpSubKey);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "RegOpenKeyEx"
                        << "\nhKey\n" << hKey
                        << "\nlpSubKey(base64)\n"
                        << base64_encode((unsigned const char *) lpSubKeyUtf8.c_str(), strlen(lpSubKeyUtf8.c_str()))
                        << "\nulOptions\n" << ulOptions
                        << "\nsamDesired\n" << samDesired
                        << "\nphkResult\n" << phkResult
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) LONG
WINAPI NewRegSetValueEx(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType,
                        const BYTE *lpData, DWORD cbData) {
    LONG result = OldRegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);
    std::string lpValueNameUtf8 = wstring_to_utf8(lpValueName);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "RegSetValueEx"
                        << "\nhKey\n" << hKey
                        << "\nlpValueName(base64)\n" << base64_encode((unsigned const char *) lpValueNameUtf8.c_str(),
                                                                      strlen(lpValueNameUtf8.c_str()))
                        << "\nReserved\n" << Reserved
                        << "\ndwType\n" << dwType
                        << "\nlpData\n" << lpData
                        << "\ncbData\n" << cbData
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) LONG WINAPI NewRegCloseKey(HKEY hKey) {
    LONG result = OldRegCloseKey(hKey);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "RegCloseKey"
                        << "\nhKey\n" << hKey
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) LONG WINAPI NewRegDeleteKey(HKEY hKey, LPCWSTR lpSubKey) {
    LONG result = OldRegDeleteKey(hKey, lpSubKey);
    std::string lpSubKeyUtf8 = wstring_to_utf8(lpSubKey);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "RegDeleteKey"
                        << "\nhKey\n" << hKey
                        << "\nlpSubKey(base64)\n"
                        << base64_encode((unsigned const char *) lpSubKeyUtf8.c_str(), strlen(lpSubKeyUtf8.c_str()))
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}
extern "C" __declspec(dllexport) LONG WINAPI NewRegDeleteValue(HKEY hKey, LPCWSTR lpValueName) {
    LONG result = OldRegDeleteValue(hKey, lpValueName);
    std::string lpValueNameUtf8 = wstring_to_utf8(lpValueName);
    std::ostringstream outputStringBuilder;
    outputStringBuilder << "pid\n" << GetCurrentProcessId()
                        << "\nfuncName\n" << "RegDeleteValue"
                        << "\nhKey\n" << hKey
                        << "\nlpValueName(base64)\n" << base64_encode((unsigned const char *) lpValueNameUtf8.c_str(),
                                                                      strlen(lpValueNameUtf8.c_str()))
                        << "\nresult\n" << result;
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}

// socket操作 socket、bind、connect、send、recv、close
SOCKET (WINAPI *OldSocket)(int af, int type, int protocol) = socket;

static int (WINAPI *OldBind)(SOCKET s, const sockaddr *name, int namelen) = bind;

static int (WINAPI *OldConnect)(SOCKET s, const sockaddr *name, int namelen) = connect;

static int (WINAPI *OldSend)(SOCKET s, const char *buf, int len, int flags) = send;

static int (WINAPI *OldRecv)(SOCKET s, char *buf, int len, int flags) = recv;

static int (WINAPI *OldClose)(SOCKET s) = closesocket;

int enableTracingSocket = 1;
extern "C" __declspec(dllexport) SOCKET WINAPI NewSocket(int af, int type, int protocol) {
    SOCKET result = OldSocket(af, type, protocol);
    if (enableTracingSocket == 1) {
        enableTracingSocket = 0;
        std::ostringstream outputStringBuilder;
        outputStringBuilder << "pid\n" << GetCurrentProcessId()
                            << "\nfuncName\n" << "socket"
                            << "\naf\n" << af
                            << "\ntype\n" << type
                            << "\nprotocol\n" << protocol
                            << "\nresult\n" << result;
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingSocket = 1;
    }
    return result;
}

int enableTracingBind = 1;
extern "C" __declspec(dllexport) int WINAPI NewBind(SOCKET s, const sockaddr *name, int namelen) {
    int result = OldBind(s, name, namelen);
    if (enableTracingBind == 1) {
        enableTracingBind = 0;
        std::ostringstream outputStringBuilder;
        outputStringBuilder << "pid\n" << GetCurrentProcessId()
                            << "\nfuncName\n" << "bind"
                            << "\ns\n" << s
                            << "\nname\n" << name
                            << "\nnamelen\n" << namelen
                            << "\nresult\n" << result;
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingBind = 1;
    }
    return result;
}

int enableTracingConnect = 1;
extern "C" __declspec(dllexport) int WINAPI NewConnect(SOCKET s, const sockaddr *name, int namelen) {
    int result = OldConnect(s, name, namelen);
    if (enableTracingConnect == 1) {
        enableTracingConnect = 0;
        std::ostringstream outputStringBuilder;
        outputStringBuilder << "pid\n" << GetCurrentProcessId()
                            << "\nfuncName\n" << "connect"
                            << "\ns\n" << s
                            << "\nname\n" << name
                            << "\nnamelen\n" << namelen
                            << "\nresult\n" << result;
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingConnect = 1;
    }
    return result;
}

int enableTracingSend = 1;
extern "C" __declspec(dllexport) int WINAPI NewSend(SOCKET s, const char *buf, int len, int flags) {
    int result = OldSend(s, buf, len, flags);
    if (enableTracingSend == 1) {
        enableTracingSend = 0;
        std::ostringstream outputStringBuilder;
        outputStringBuilder << "pid\n" << GetCurrentProcessId()
                            << "\nfuncName\n" << "send"
                            << "\ns\n" << s
                            << "\nbuf(base64)\n" << base64_encode((const unsigned char *) buf, len)
                            << "\nlen\n" << len
                            << "\nflags\n" << flags
                            << "\nresult\n" << result;
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingSend = 1;
    }
    return result;
}

int enableTracingRecv = 1;
extern "C" __declspec(dllexport) int WINAPI NewRecv(SOCKET s, char *buf, int len, int flags) {
    int result = OldRecv(s, buf, len, flags);
    if (enableTracingRecv == 1) {
        enableTracingRecv = 0;
        std::ostringstream outputStringBuilder;
        outputStringBuilder << "pid\n" << GetCurrentProcessId()
                            << "\nfuncName\n" << "recv"
                            << "\ns\n" << s
                            << "\nbuf(base64)\n" << base64_encode((const unsigned char *) buf, len)
                            << "\nlen\n" << len
                            << "\nflags\n" << flags
                            << "\nresult\n" << result;
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingRecv = 1;
    }
    return result;
}

int enableTracingClose = 1;
extern "C" __declspec(dllexport) int WINAPI NewClose(SOCKET s) {
    int result = OldClose(s);
    if (enableTracingClose == 1) {
        enableTracingClose = 0;
        std::ostringstream outputStringBuilder;
        outputStringBuilder << "pid\n" << GetCurrentProcessId()
                            << "\nfuncName\n" << "close"
                            << "\ns\n" << s
                            << "\nresult\n" << result;
        sendUdpPacked(outputStringBuilder.str().c_str());
        enableTracingClose = 1;
    }
    return result;
}

void StartHook() {
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // 弹窗
    DetourAttach(&(PVOID &) OldMessageBoxA, NewMessageBoxA);
    DetourAttach(&(PVOID &) OldMessageBoxW, NewMessageBoxW);
    // 文件操作
    DetourAttach(&(PVOID &) OldCreateFileA, NewCreateFileA);
    DetourAttach(&(PVOID &) OldWriteFile, NewWriteFile);
    DetourAttach(&(PVOID &) OldReadFile, NewReadFile);
//    DetourAttach(&(PVOID &) OldCloseHandle, NewCloseHandle);
    // 堆操作
    DetourAttach(&(PVOID &) OldHeapCreate, NewHeapCreate);
    DetourAttach(&(PVOID &) OldHeapAlloc, NewHeapAlloc);
    DetourAttach(&(PVOID &) OldHeapFree, NewHeapFree);
    DetourAttach(&(PVOID &) OldHeapDestroy, NewHeapDestroy);
    // 注册表操作
    DetourAttach(&(PVOID &) OldRegCreateKeyEx, NewRegCreateKeyEx);
    DetourAttach(&(PVOID &) OldRegOpenKeyEx, NewRegOpenKeyEx);
    DetourAttach(&(PVOID &) OldRegSetValueEx, NewRegSetValueEx);
    DetourAttach(&(PVOID &) OldRegCloseKey, NewRegCloseKey);
    DetourAttach(&(PVOID &) OldRegDeleteKey, NewRegDeleteKey);
    DetourAttach(&(PVOID &) OldRegDeleteValue, NewRegDeleteValue);
    // socket操作
    DetourAttach(&(PVOID &) OldSocket, NewSocket);
    DetourAttach(&(PVOID &) OldBind, NewBind);
    DetourAttach(&(PVOID &) OldConnect, NewConnect);
    DetourAttach(&(PVOID &) OldSend, NewSend);
    DetourAttach(&(PVOID &) OldRecv, NewRecv);
    DetourAttach(&(PVOID &) OldClose, NewClose);
    DetourTransactionCommit();
}

//解除钩子
void EndHook() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // 弹窗
    DetourDetach(&(PVOID &) OldMessageBoxA, NewMessageBoxA);
    DetourDetach(&(PVOID &) OldMessageBoxW, NewMessageBoxW);
    // 文件操作
    DetourDetach(&(PVOID &) OldCreateFileA, NewCreateFileA);
    DetourDetach(&(PVOID &) OldWriteFile, NewWriteFile);
    DetourDetach(&(PVOID &) OldReadFile, NewReadFile);
//    DetourDetach(&(PVOID &) OldCloseHandle, NewCloseHandle);
    // 堆操作
    DetourDetach(&(PVOID &) OldHeapCreate, NewHeapCreate);
    DetourDetach(&(PVOID &) OldHeapAlloc, NewHeapAlloc);
    DetourDetach(&(PVOID &) OldHeapFree, NewHeapFree);
    DetourDetach(&(PVOID &) OldHeapDestroy, NewHeapDestroy);
    // 注册表操作
    DetourDetach(&(PVOID &) OldRegCreateKeyEx, NewRegCreateKeyEx);
    DetourDetach(&(PVOID &) OldRegOpenKeyEx, NewRegOpenKeyEx);
    DetourDetach(&(PVOID &) OldRegSetValueEx, NewRegSetValueEx);
    DetourDetach(&(PVOID &) OldRegCloseKey, NewRegCloseKey);
    DetourDetach(&(PVOID &) OldRegDeleteKey, NewRegDeleteKey);
    DetourDetach(&(PVOID &) OldRegDeleteValue, NewRegDeleteValue);
    // socket操作
    DetourDetach(&(PVOID &) OldSocket, NewSocket);
    DetourDetach(&(PVOID &) OldBind, NewBind);
    DetourDetach(&(PVOID &) OldConnect, NewConnect);
    DetourDetach(&(PVOID &) OldSend, NewSend);
    DetourDetach(&(PVOID &) OldRecv, NewRecv);
    DetourDetach(&(PVOID &) OldClose, NewClose);
    DetourTransactionCommit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            //避免被重复加载
//            if (GetModuleHandleA("HookDemo.dll") != nullptr) {
//                return TRUE;
//            }
            heapCreateLock = TlsAlloc();
            heapAllocLock = TlsAlloc();
            heapFreeLock = TlsAlloc();
            heapDestroyLock = TlsAlloc();
            TlsSetValue(heapCreateLock, 0);
            TlsSetValue(heapAllocLock, (PVOID) 0);
            TlsSetValue(heapFreeLock, (PVOID) 0);
            TlsSetValue(heapDestroyLock, (PVOID) 0);
            initUDPClient();
            StartHook();
            break;
        case DLL_THREAD_ATTACH:
            heapCreateLock = TlsAlloc();
            heapAllocLock = TlsAlloc();
            heapFreeLock = TlsAlloc();
            heapDestroyLock = TlsAlloc();
            TlsSetValue(heapCreateLock, 0);
            TlsSetValue(heapAllocLock, (PVOID) 0);
            TlsSetValue(heapFreeLock, (PVOID) 0);
            TlsSetValue(heapDestroyLock, (PVOID) 0);
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
