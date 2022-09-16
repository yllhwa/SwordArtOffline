//
// Created by yll20 on 2022/9/14.
//

#ifndef HOOKDEMO_HOOKMESSAGEBOX_H
#define HOOKDEMO_HOOKMESSAGEBOX_H

#include "common.h"
#include "../UDPClient/UDPClient.h"
#include "../Utils/utils.h"
#include "../Locale/Locale.h"
#include "../Base64/base64.h"
#include "../LuaEngine/LuaEngine.h"
#include <Windows.h>

#define _export extern "C" __declspec(dllexport)

// 弹窗 MessageBoxA、MessageBoxW
static int (WINAPI *OldMessageBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) = MessageBoxA;

static int (WINAPI *OldMessageBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) = MessageBoxW;

// 弹窗实现
_export int WINAPI NewMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    const std::string funcArgs[] = {"funcName", "hWnd", "lpText(base64)", "lpCaption(base64)", "uType", "result"};
    LuaEnter("EnterMessageBoxA", hWnd, lpText, lpCaption, uType);

    int result = OldMessageBoxA(hWnd, lpText, lpCaption, uType);
    // 构建消息
    std::ostringstream outputStringBuilder;
    // lpText和lpCation为gbk编码，需要转换为utf8编码
    std::string strText = GbkToUtf8(lpText);
    std::string strCaption = GbkToUtf8(lpCaption);
    // base64编码
    std::string base64Text = base64_encode((const unsigned char *) strText.c_str(), strlen(strText.c_str()));
    std::string base64Caption = base64_encode((const unsigned char *) strCaption.c_str(), strlen(strCaption.c_str()));
    buildMessage(outputStringBuilder, funcArgs, "MessageBoxA", hWnd, base64Text, base64Caption, uType, result);
    // 发送消息
    sendUdpPacked(outputStringBuilder.str().c_str());

    LuaLeave("LeaveMessageBoxA", result);
    return result;
}

_export int WINAPI NewMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    const std::string funcArgs[] = {"funcName", "hWnd", "lpText(base64)", "lpCaption(base64)", "uType", "result"};

    int result = OldMessageBoxW(hWnd, lpText, lpCaption, uType);
    // 构建消息
    std::ostringstream outputStringBuilder;
    // lpText和lpCaption都是Unicode字符串，转为UTF-8字符串
    std::string lpTextUtf8 = wstring_to_utf8(lpText);
    std::string lpCaptionUtf8 = wstring_to_utf8(lpCaption);
    // base64编码
    std::string base64Text = base64_encode((const unsigned char *) lpTextUtf8.c_str(), strlen(lpTextUtf8.c_str()));
    std::string base64Caption = base64_encode((const unsigned char *) lpCaptionUtf8.c_str(), strlen(lpCaptionUtf8.c_str()));
    buildMessage(outputStringBuilder, funcArgs, "MessageBoxW", hWnd, base64Text, base64Caption, uType, result);
    // 发送消息
    sendUdpPacked(outputStringBuilder.str().c_str());
    return result;
}

#endif //HOOKDEMO_HOOKMESSAGEBOX_H
