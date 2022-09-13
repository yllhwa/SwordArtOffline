//
// Created by yll20 on 2022/8/29.
//

#ifndef HOOKDEMO_LOCALE_H
#define HOOKDEMO_LOCALE_H

#include <string>
#include <cstring>
std::string GbkToUtf8(const char *src_str);
std::string Utf8ToGbk(const char *src_str);
std::string wstring_to_utf8(const std::wstring& wstr);
#endif //HOOKDEMO_LOCALE_H
