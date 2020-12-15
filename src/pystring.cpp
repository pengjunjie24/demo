
#include "pystring.h"

#include <string.h>

namespace
{
    const std::string s_forward_slash = "/";
    const std::string s_double_forward_slash = "//";
    const std::string s_triple_forward_slash = "///";
    const std::string s_double_back_slash = "\\";
    const std::string s_empty_string = "";
    const std::string s_dot = ".";
    const std::string s_double_dot = "..";
    const std::string s_colon = ":";

    //调整[start, end]的位置
    static void _adjust_indices(const int32_t len, int32_t& start, int32_t& end)
    {
        if (end > len)
        {
            end = len;
        }
        else if (end < 0)
        {
            end += len;
            if (end < 0)
            {
                end = 0;
            }
        }

        if (start < 0)
        {
            start += len;
            if (start < 0)
            {
                start = 0;
            }
        }
    }

    //字符串匹配函数,在self字符串中匹配substr字符串
    //startdirection=true,在self头上匹配substr字符，否则在self结束位置匹配substr字符
    bool _string_tailmatch(
        const std::string& self,
        const std::string& substr,
        int32_t start,
        int32_t end,
        bool startdirection)
    {
        int32_t len = static_cast<int32_t>(self.size());
        int32_t slen = static_cast<int32_t>(substr.size());

        const char* sub = substr.c_str();
        const char* str = self.c_str();

        _adjust_indices(len, start, end);

        if (startdirection)
        {
            //start位置加上子串位置大于self字符串长度，不匹配
            if (start + slen > len)
            {
                return false;
            }
        }
        else
        {
            if (end - start < slen || start > len)
            {
                return false;
            }
            if (end - slen > start)
            {
                start = end - slen;
            }
        }

        if (end - start >= slen)
        {
            return (!memcmp(str + start, sub, slen));
        }

        return 0;
    }
}

namespace pystring
{
    int32_t find(const std::string& str, const std::string& sub, int32_t start, int32_t end)
    {
        _adjust_indices(static_cast<int32_t>(str.size()), start, end);
        std::string::size_type result = str.find(sub, start);

        if (result == std::string::npos ||
            (result + sub.size() > static_cast<std::string::size_type>(end)))
        {
            return -1;
        }
        return static_cast<int32_t>(result);
    }

    int32_t count(const std::string& str,
        const std::string& substr, int32_t start, int32_t end)
    {
        int32_t num_matches = 0;
        int32_t cursor = start;

        while (true)
        {
            cursor = find(str, substr, cursor, end);
            if (cursor < 0)
            {
                break;
            }

            cursor += static_cast<int32_t>(substr.size());
            num_matches += 1;
        }

        return num_matches;
    }

    bool endswith(const std::string& str,
        const std::string& suffix,
        int start = 0,
        int32_t end = std::numeric_limits<int32_t>::max())
    {
        return _string_tailmatch(str, suffix, start, end, false);
    }

    bool startswith(const std::string& str,
        const std::string& suffix,
        int start = 0,
        int32_t end = std::numeric_limits<int32_t>::max())
    {
        return _string_tailmatch(str, suffix, start, end, true);
    }

    std::string expandtabs(const std::string& str, const int32_t tabsize = 8)
    {
        std::string copy_str{str};
        std::string::size_type strlen = str.size();
        int32_t strindex = 0;//源字符串str当前遍历位置
        int32_t copyindex = 0;//copy字符串当前遍历行位置
        int32_t offset = 0;//源字符串str在修改时需要偏移的位置
        for(; strindex < strlen; ++strindex)
        {
            if (str[strindex] == '\t')
            {
                //填充空格数大于0
                if (tabsize > 0)
                {
                    int32_t fillsize = tabsize - (copyindex % tabsize);//需要填充空格个数
                    copyindex += fillsize;
                    copy_str.replace(strindex + offset, 1, std::string(fillsize, ' '));
                    offset += fillsize - 1;
                }
                else
                {
                    copy_str.replace(strindex + offset, 1, s_empty_string);
                    offset -= 1;
                }
            }
            else
            {
                ++copyindex;
                //换行时行位置重新归零
                if (str[strindex] == '\n' || str[strindex] == '\r')
                {
                    copyindex = 0;
                }
            }
        }
    }
    
    bool strisfunc(const std::string& str, str_judgment_func strfunc)
    {
        if (str.empty())
        {
            return false;
        }

        std::string::size_type strlen = str.size();
        for (std::string::size_type i = 0; i < strlen; ++i)
        {
            if (!strfunc(str[i]))
            {
                return false;
            }
        }
        return true;
    }

    std::string lower(const std::string& str)
    {
        std::string low_str(str);
        int32_t strlen = static_cast<int32_t>(low_str.size());
        for (int32_t i = 0; i < strlen; ++i)
        {
            if (::isupper(low_str[i]))
            {
                low_str[i] = static_cast<char>(::tolower(low_str[i]));
            }
        }

        return low_str;
    }

    std::string upper(const std::string& str)
    {
        std::string upper_str(str);
        int32_t strlen = static_cast<int32_t>(upper_str.size());
        for (int32_t i = 0; i < strlen; ++i)
        {
            if (::islower(upper_str[i]))
            {
                upper_str[i] = static_cast<char>(::toupper(upper_str[i]));
            }
        }

        return upper_str;
    }

    std::string swapcase(const std::string& str)
    {
        std::string swap_str(str);
        int32_t strlen = static_cast<int32_t>(swap_str.size());

        for (int32_t i = 0; i < strlen; ++i)
        {
            if (::islower(swap_str[i]))
            {
                swap_str[i] = static_cast<char>(::toupper(swap_str[i]));
            }
            else if (::isupper(swap_str[i]))
            {
                swap_str[i] = static_cast<char>(::tolower(swap_str[i]));
            }
        }
        return swap_str;
    }

    std::string join(const std::vector<std::string>& seq, const std::string& str)
    {
        if (seq.empty())
        {
            return s_empty_string;
        }

        int32_t seqSize = static_cast<int32_t>(seq.size());
        std::string result(seq[0]);
        if (seqSize ==  1)
        {
            return result;
        }
        for (int32_t i = 1; i < seqSize; ++i)
        {
            result += (str + seq[i]);
        }
        return result;
    }

    std::string ljust(const std::string& str, int32_t width)
    {
        int32_t strlen = static_cast<int32_t>(str.size());
        if (strlen >= width)
        {
            return str;
        }
        return str + std::string(width - strlen, ' ');
    }

    std::string rjust(const std::string& str, int32_t width)
    {
        int32_t strlen = static_cast<int32_t>(str.size());
        if (strlen >= width)
        {
            return str;
        }
        return std::string(width - strlen, ' ') + str;
    }

    std::string center(const std::string& str, int32_t width)
    {
        int32_t strlen = static_cast<int32_t>(str.size());
        if (strlen >= width)
        {
            return str;
        }
        
        int32_t marg = width - strlen;
        int32_t left = marg / 2 + (marg & width & 1);
        return std::string(left, ' ') + str + std::string(marg - left, ' ');
    }
}