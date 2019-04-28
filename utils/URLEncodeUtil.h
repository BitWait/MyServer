/**
 * URL±à½âÂë¹¤¾ß£¬URLEncodeUtil.h
 * zhangyl 2018.05.16
 */
#ifndef __URLENCODE_UTIL_H__
#define __URLENCODE_UTIL_H__
#include <string>

class URLEncodeUtil final
{
private:
    URLEncodeUtil() = delete;
    ~URLEncodeUtil() = delete;

    URLEncodeUtil(const URLEncodeUtil& rhs) = delete;
    URLEncodeUtil& operator =(const URLEncodeUtil& rhs) = delete;

public:
    static bool Encode(const std::string& src, std::string& dst);
    static bool Decode(const std::string& src, std::string& dst);
	static std::string URLEncode(const std::string &sIn);
	static std::string URLDecode(const std::string &sIn);

};

#endif //!__URLENCODE_UTIL_H__