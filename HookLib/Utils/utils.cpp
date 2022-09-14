//
// Created by yll20 on 2022/9/14.
//

#include "utils.h"
#include <sstream>
#include <processthreadsapi.h>
#include <libloaderapi.h>

std::string getFilenameByPath(LPCSTR path) {
    std::string strPath = path;
    std::string::size_type pos = strPath.find_last_of('\\');
    // 还有可能是/
    if (pos == std::string::npos) {
        pos = strPath.find_last_of('/');
    }
    if (pos != std::string::npos) {
        return strPath.substr(pos + 1);
    }
    return strPath;
}

std::string getSelfFileName() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    return getFilenameByPath(buffer);
}