#include "../LogStream.h"
#include <string>
#include <iostream>
using namespace detail;
int main()
{
	LogStream os;
	const LogStream::Buffer &buf = os.buffer();
	std::cout << "------------------begin test bool----------------------" << std::endl;
	os << true;
	std::cout << buf.asString().c_str() << std::endl;

	os << false;
	std::cout << buf.asString().c_str() << std::endl;

	os.resetBuffer();
	//std::cout << buf.asString().c_str() << std::endl;

	std::cout << "------------------end test bool----------------------" << std::endl;

	std::cout << "------------------begin test integer----------------------" << std::endl;
	os<< 1;
	std::cout << buf.asString().c_str() << std::endl;
	os << 0;
	std::cout << buf.asString().c_str() << std::endl;
	os << -1;
	std::cout << buf.asString().c_str() << std::endl;
	os.resetBuffer();
	//std::cout << buf.asString().c_str() << std::endl;
	std::cout << "------------------end test integer----------------------" << std::endl;

	std::cout << "------------------begin test fmt----------------------" << std::endl;
	os << Fmt("%4d", 1);
	std::cout << buf.asString().c_str() << std::endl;
	os.resetBuffer();

	os << Fmt("%4.2f", 1.2);
	std::cout << buf.asString().c_str() << std::endl;
	os.resetBuffer();

	os << Fmt("%4.2f", 1.2) << Fmt("%4d", 43);
	std::cout << buf.asString().c_str() << std::endl;
	os.resetBuffer();
	std::cout << "------------------end test fmt----------------------" << std::endl;
}