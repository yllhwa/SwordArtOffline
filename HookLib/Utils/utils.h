//
// Created by yll20 on 2022/9/14.
//

#ifndef HOOKDEMO_UTILS_H
#define HOOKDEMO_UTILS_H

#include <sstream>
#include <Windows.h>

template<typename T>
std::ostringstream &buildMessage(std::ostringstream &os, const std::string funcArgs[], const T &param) {
    os << funcArgs[0] << "\n" << param;
    return os;
}

template<typename T, typename... Args>
std::ostringstream &
buildMessage(std::ostringstream &os, const std::string funcArgs[], const T &param, const Args &...rest) {
    // 若funcArgs[0]为funcName，则添加pid
    if (funcArgs[0] == "funcName") {
        os << "pid" << "\n" << GetCurrentProcessId() << "\n";
    }
    os << funcArgs[0] << "\n" << param << "\n";
    return buildMessage(os, funcArgs + 1, rest...);
}

std::string getFilenameByPath(LPCSTR path);
std::string getSelfFileName();

#endif //HOOKDEMO_UTILS_H
