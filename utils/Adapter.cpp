#include "Adapter.h"
#include <string.h>
#include <iconv.h>
#include <stdlib.h>
#include <memory>

#define MIN(a,b) ((a)>(b)?(b):(a))

//**********************************
//Description: 转换为宽字符模式
// Parameter:  参照MSDN
// Returns:    int 返回被转换的字节数
//              失败返回0
//              暂时支持gbk和utf8两种
//注:cchWideChar为0时表示要取需要缓冲区大小，
//此时linux版本下最大可支持转换后宽字符长度不超过8K的操作
//************************************
inline int MultiByteToWideChar(
	unsigned int CodePage,         // code page
	unsigned long dwFlags,         // character-type options
	const char* lpMultiByteStr, // string to map
	int cbMultiByte,       // number of bytes in string
	wchar_t* lpWideCharStr,  // wide-character buffer
	int cchWideChar        // size of buffer
	)
{
	int iRet = 0;	
	iconv_t cd;
	int rc;
	char **pin = (char **)&lpMultiByteStr;
	char **pout = (char**)&lpWideCharStr;

	size_t tWideChar = cchWideChar*sizeof(wchar_t);
	size_t tMultiByte = strlen(lpMultiByteStr);

	if (65001 == CodePage)
	{
		cd = iconv_open("UCS-4LE", "UTF-8");
	}
	else
	{
		cd = iconv_open("UCS-4LE", "gb18030");
	}
	if (cd == (void*)0xffffffffffffffff)
	{
		//printf("%s %d\n", strerror(errno), errno);
		return 0;
	}
	//获取所需缓冲区大小
	if (0 == cchWideChar)
	{
		std::wstring dest(tMultiByte + 1, '\0');
		size_t tDestWideChar = dest.size()*sizeof(wchar_t);
		char *pdest = (char*)dest.data();
		char **ppdest = &pdest;

		if (iconv(cd, pin, &tMultiByte, ppdest, &tDestWideChar) == (size_t)-1)
		{
			iconv_close(cd);
			return 0;
		}
		iRet = (dest.size()*sizeof(wchar_t)-tDestWideChar) / sizeof(wchar_t);
	}
	else
	{
		if (iconv(cd, pin, &tMultiByte, pout, (size_t*)&tWideChar) == (size_t)-1)
		{
			iconv_close(cd);
			return 0;
		}
		iRet = (cchWideChar*sizeof(wchar_t)-tWideChar) / sizeof(wchar_t);
	}
	iconv_close(cd);

	return iRet;
}

//**********************************
//Description: 转换为多字节模式
// Parameter:  参照MSDN
// Returns:    int 返回被转换的字节数
//              失败返回0
//              暂时支持gbk和utf8两种
//注:cbMultiByte为0时表示要取需要缓冲区大小，
//此时linux版本下最大可支持转换后宽字符长度不超过8K的操作
//************************************
inline int WideCharToMultiByte(
	unsigned int CodePage,            // code page
	unsigned long dwFlags,            // performance and mapping flags
	const wchar_t*  lpWideCharStr,    // wide-character string
	int cchWideChar,          // number of chars in string
	char* lpMultiByteStr,     // buffer for new string
	int cbMultiByte,          // size of buffer
	const char* lpDefaultChar,     // default for unmappable chars
	int lpUsedDefaultChar  // set when default char used
	)
{
	int iRet = 0;
	iconv_t cd;
	int rc;
	char **pin = (char **)&lpWideCharStr;
	char **pout = (char**)&lpMultiByteStr;
	size_t tWideChar = wcslen(lpWideCharStr)*sizeof(wchar_t);
	size_t tMultiByte = cbMultiByte;

	if (65001 == CodePage)
	{
		cd = iconv_open("UTF-8", "UCS-4LE");
	}
	else
	{
		cd = iconv_open("gb18030", "UCS-4LE");
	}

	if (cd == (void*)0xffffffffffffffff)
	{
		//printf("%s %d\n", strerror(errno), errno);
		return 0;
	}

	//获取所需缓冲区大小
	if (0 == cbMultiByte)
	{
		// char dest[8192+1] = {0};
		char dest[15 * 1024 + 1] = { 0 };
		size_t tDestMultiByte = sizeof(dest)-1;
		char *pdest = dest;
		char **ppdest = &pdest;
		if (iconv(cd, pin, &tWideChar, ppdest, &tDestMultiByte) == (size_t)-1)
		{
			iconv_close(cd);
			return 0;
		}
		iRet = sizeof(dest)-1 - tDestMultiByte;
	}
	else
	{
		if (iconv(cd, pin, (size_t*)&tWideChar, pout, (size_t*)&tMultiByte) == (size_t)-1)
		{
			iconv_close(cd);
			return 0;
		}
		iRet = cbMultiByte - tMultiByte;

	}
	iconv_close(cd);

	return iRet;
}


void CAdapter::ConvertGBKToUTF8(std::string& strGBK)
{
	int len = MultiByteToWideChar(0, 0, strGBK.c_str(), -1, NULL, 0);
	unsigned int * wszUtf8 = new unsigned int[len + 1];
	char *szUtf8 = NULL;
	if (wszUtf8)
	{
		try
		{
			try
			{
				memset(wszUtf8, 0, len * 4 + 4);
				MultiByteToWideChar(0, 0, strGBK.c_str(), -1, (wchar_t*)wszUtf8, len);
				len = WideCharToMultiByte(65001, 0, (const wchar_t*)wszUtf8, -1, NULL, 0, NULL, 0);
			}
			catch (...)
			{
				if (wszUtf8)
				{
					delete[] wszUtf8;
					wszUtf8 = NULL;
				}
			}
			if (wszUtf8)
			{
				szUtf8 = new char[len + 1];
				if (szUtf8)
				{
					try
					{
						memset(szUtf8, 0, len + 1);
						WideCharToMultiByte(65001, 0, (const wchar_t*)wszUtf8, -1, szUtf8, len, NULL, 0);
						strGBK = szUtf8;
					}
					catch (...)
					{
					}
				}
				if (szUtf8)
				{
					delete[] szUtf8;
					szUtf8 = NULL;
				}
			}
			if (wszUtf8)
			{
				delete[] wszUtf8;
				wszUtf8 = NULL;
			}
		}
		catch (...)
		{
			if (wszUtf8)
			{
				delete[] wszUtf8;
				wszUtf8 = NULL;
			}
			if (szUtf8)
			{
				delete[] szUtf8;
				szUtf8 = NULL;
			}
		}
	}
}

std::string CAdapter::ConvertUTF8ToGBK(const std::string strSrc)
{
	iconv_t cd;
	char strTmpSrc[1024] = { 0 };
	char strTmpDest[1024] = { 0 };
	strncpy(strTmpSrc, strSrc.c_str(), std::min(sizeof(strTmpSrc)-1, strSrc.length()));
	char * pSrc = strTmpSrc;
	char * pDest = strTmpDest;
	char **pin = &pSrc;
	char **pout = &pDest;
	try
	{
		
		cd = iconv_open("gb18030", "UTF-8");

		if (cd == (void*)0xffffffffffffffff)
		{
			return strTmpDest;
		}

		size_t outLen = strSrc.length();
		if (iconv(cd, pin, &outLen, pout, &outLen) == (size_t)-1)
		{
			iconv_close(cd);
			return strTmpDest;
		}
		iconv_close(cd);
	}
	catch (...)
	{
	}
	return strTmpDest;
}