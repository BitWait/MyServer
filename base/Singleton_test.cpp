#include "singleton.h"
#include <stdio.h>
#include <string>
#include <thread>


class Test
{
public:
	Test(){ printf("constructing!\n"); }
	~Test(){ printf("destruting!\n"); }

	const std::string& name(){ return name_; }
	void setName(const std::string& name){ name_ = name; }
private:
	std::string name_;
};


int main(void)
{
	Singleton<Test>::Instance().setName("Hello");
	printf("name_ = %s\n", Singleton<Test>::Instance().name().c_str());
	
}