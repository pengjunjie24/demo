
#pragma once

#include <limits>
#include <string>
#include <memory>
#include <vector>

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
    bool endswith(const std::string& str,
        const std::string& suffix,
        int start = 0,
        int32_t end = std::numeric_limits<int32_t>::max());

    //字符串str以指定后缀suffix开始，则返回true,否则返回false
    bool startswith(const std::string& str,
        const std::string& suffix,
        int start = 0,
        int32_t end = std::numeric_limits<int32_t>::max());

    //返回字符串str拷贝, 原有str中的制表符变为tabsize个空格, 空格大小不设置默认为8
    std::string expandtabs(const std::string& str, const int32_t tabsize = 8);

    //判断字符串的函数
    using str_judgment_func = std::function<int(int)>;
    //判断字符串中字符类型，str是需要判断的字符串，strfunc是判断函数
    //strfunc可以为以下类型:
    //isalnum 判断是否全为字母数字
    //isalpha 判断是否全为字母
    //isdigit 判断是否全为数字
    //islower 判断是否全为小写字符
    //isupper 判断是否全为大写字符
    //isspace 判断是否全为空格字符
    bool strisfunc(const std::string& str, str_judgment_func strfunc);

    //返回将str字符串中所有字母转为小写的字符串
    std::string lower(const std::string& str);

    //返回将str字符串中所有字母转为大写的字符串
    std::string upper(const std::string & str);

    //返回将str字符串中小写字母转为大写字母，反之亦然
    std::string swapcase(const std::string & str);

    //将容器seq中的字符串连接，seq中字符串连接分隔符为str参数，并返回连接完成字符串
    std::string join(const std::vector<std::string>& seq, const std::string& str);
    
    //返回左对齐的str字符串，如果width小于str长度，则返回str
    //否则用空格填充剩余长度
    std::string ljust(const std::string& str, int32_t width);

    //返回右对齐字符串
    std::string rjust(const std::string& str, int32_t width);

    //返回中间对齐字符串
    std::string center(const std::string & str, int32_t width);
}