#ifndef __ADAPTER_UTIL_H__
#define __ADAPTER_UTIL_H__
#include <string>

class CAdapter final
{
private:
	CAdapter() = delete;
	~CAdapter() = delete;

	CAdapter(const CAdapter& rhs) = delete;
	CAdapter& operator =(const CAdapter& rhs) = delete;

public:
	static void ConvertGBKToUTF8(std::string& strGBK);
	static std::string ConvertUTF8ToGBK(const std::string strSrc);

};
#endif