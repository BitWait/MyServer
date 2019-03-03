#pragma once
#include <string.h>

namespace detail
{
	const int kSmallBuffer = 4000;
	const int kLargeBuffer = 4000 * 1000;

	template<int SIZE>
	class FixedBuffer
	{
	public:
		FixedBuffer():cur_(data_)
		{
			setCookie(cookieStart);
		}
		~FixedBuffer()
		{
			setCookie(cookieEnd);
		}

		void setCookie(void(*cookie)())
		{ 
			cookie_ = cookie; 
		}

		void append(const char * buf, size_t len)
		{
			if (avail() > static_cast<int>(len))
			{
				memcpy(cur_, buf, len);
				cur_ += len;
			}
		}


		// write to data_ directly
		int avail()const
		{
			return static_cast<int>(end() - cur_);
		}

	private:
		const char * end()const
		{
			return data_ + sizeof data_;
		}
		// Must be outline function for cookies.
		static void cookieStart();
		static void cookieEnd();
		void(*cookie_)();
		char *cur_;
		char data_[SIZE];
	};

};
class LogStream
{
	typedef LogStream self;
public:
	typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

	self& operator<<(bool v)
	{
		buffer_.append(v ? "1" : "0", 1);
		return *this;
	}

private:
	Buffer buffer_;
	static const int kMaxNumericSize = 32;
};

