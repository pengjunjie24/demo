
#include "pystring.h"

namespace
{
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

    int32_t _string_tailmatch(const std::string& self,
        const std::string& substr,
        int32_t start,
        int32_t end,
        int32_t direction)
    {
        int32_t len = static_cast<int32_t>(self.size());
        int32_t slen = static_cast<int32_t>(substr.size());

        const char* sub = substr.c_str();
        const char* str = self.c_str();

        _adjust_indices(len, start, end);

        if (direction < 0)
        {
            if (start + slen > len)
            {
                return 0;
            }
        }
        else
        {
            if (end - start  < slen || start > len)
            {
                return 0;
            }
            if (end - slen > start)
            {
                start = end - slen;
            }
        }

        if (end - start >= slen)
        {
            return (!std::memcmp(str + start, sub, slen));
        }

        return 0;
    }
}

namespace pystring
{

    int32_t find(const std::string& str,
        const std::string& sub, int32_t start, int32_t end)
    {
        _adjust_indices(static_cast<int32_t>(str.size()), start, end);
        auto result = str.find(sub, start);

        if (result == std::string::npos ||
            (result + sub.size() > (std::string::size_type)end))
        {
            return -1;
        }

        return static_cast<int32_t>(result);
    }

    int32_t count(const std::string& str,
        const std::string& substr, int32_t start, int32_t end)
    {
        int32_t numMatches = 0;
        int32_t cursor = start;

        while (true)
        {
            cursor = find(str, substr, cursor, end);
            if (cursor < 0)
            {
                break;
            }

            cursor += static_cast<int32_t>(substr.size());
            numMatches += 1;
        }

        return numMatches;
    }

    bool endwith(const std::string& str,
        const std::string& suffix,
        int start = 0,
        int32_t end = std::numeric_limits<int32_t>::max())
    {

    }
}