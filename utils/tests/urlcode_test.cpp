#include "../URLEncodeUtil.h"
#include<iostream>
using namespace std;

int main(void)
{
	std::string srcEncode = "{\"username\": \"13917043329\", \"nickname\": \"balloon\", \"password\":\"123\"}";
	std::string dstDecode;
	URLEncodeUtil::Encode(srcEncode, dstDecode);
	std::cout << dstDecode << std::endl;
	std::string src;
	URLEncodeUtil::Decode(dstDecode, src);
	std::cout << src << std::endl;
}