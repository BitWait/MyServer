#include "../Adapter.h"
#include <string>
#include <iostream>
#include <string.h>

int main(void)
{
	std::string str = "雪豹是最严格的小组，拥有最高的淘汰率！";

	std::string strDest;
	CAdapter::ConvertGBKToUTF8(str);
	std::cout << str << std::endl;
	strDest = CAdapter::ConvertUTF8ToGBK(str);

	std::cout << strDest << std::endl;
	return 0;
}