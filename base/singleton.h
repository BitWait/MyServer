#pragma once
#include <stdlib.h>
template<typename T>
class Singleton
{
public:
	
	static T& Instance()
	{
		if (NULL == value_ )
		{
			value_ = new T();
		}
		return *value_;
	}

private:
	Singleton();
	~Singleton();

	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

	static void init()
	{
		value_ = new T();
	}
	static void destroy()
	{
		delete value_;
	}
private:
	static T* value_;
};

template<typename T>
T* Singleton<T>::value_ = NULL;