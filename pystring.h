
#pragma once

#include <limits>
#include <string>

#include <stdint.h>

namespace pystring
{
    //返回找到字符串str中[start, end]子字符串sub的字符串的最低索引
    //没有找到sub，则返回-1
    int32_t find(const std::string& str,
        const std::string& sub,
        int32_t start = 0,
        int32_t end = std::numeric_limits<int32_t>::max());

    //返回字符串str在[start, end]子串出现的次数
    int32_t count(const std::string& str,
        const std::string& substr,
        int32_t start = 0,
        int32_t end = std::numeric_limits<int32_t>::max());

    //字符串str以指定后缀suffix结束，则返回true,否则返回false
    bool endwith(const std::string& str,
        const std::string& suffix,
        int start = 0,
        int32_t end = std::numeric_limits<int32_t>::max());
}