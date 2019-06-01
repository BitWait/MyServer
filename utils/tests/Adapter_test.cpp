#include "../Adapter.h"
#include <string>
#include <iostream>
#include <string.h>

int main(void)
{
	std::string str = "ѩ�������ϸ��С�飬ӵ����ߵ���̭�ʣ�";

	std::string strDest;
	CAdapter::ConvertGBKToUTF8(str);
	std::cout << str << std::endl;
	strDest = CAdapter::ConvertUTF8ToGBK(str);

	std::cout << strDest << std::endl;
	return 0;
}