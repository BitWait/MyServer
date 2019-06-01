/**
 * 字符串操作工具类, StringUtil.cpp
 * zhangyl 2018.03.09
 */
#include "StringUtil.h"
#include <sstream>

void StringUtil::Split(const std::string& str, std::vector<std::string>& v, const char* delimiter/* = "|"*/)
{
    if (delimiter == NULL || delimiter[0] == '\0' || str.empty())
        return;

    std::string buf = str;
    size_t pos = std::string::npos;
    std::string substr;
    while (true)
    {
        pos = buf.find(delimiter);
        if (pos != std::string::npos)
        {
            substr = buf.substr(0, pos);
            if (!substr.empty())
                v.push_back(substr);

            buf = buf.substr(pos + 1);
        }
        else
        {
            v.push_back(buf);
            break;
        }           
    }
}

std::string StringUtil::Int2String(uint32_t user_id)
{
	std::stringstream ss;
	ss << user_id;
	return ss.str();
}

uint32_t StringUtil::String2Int(const std::string& value)
{
	return (uint32_t)atoi(value.c_str());
}
