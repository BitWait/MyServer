#pragma once

#include <functional>
#include <memory>
#include "../base/timestamp.h"

// Adapted from google-protobuf stubs/common.h

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
	return f;
}

template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f)                     // so we only accept pointers
{
	// Ensures that To is a sub-type of From *.  This test is here only
	// for compile-time type checking, and has no overhead in an
	// optimized build at run-time, as it will be optimized away
	// completely.
	if (false)
	{
		implicit_cast<From*, To>(0);
	}

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
	assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
	return static_cast<To>(f);
}

template<typename To, typename From>
inline std::shared_ptr<To> down_pointer_cast(const std::shared_ptr<From>& f)
{
	if (false)
	{
		implicit_cast<From*, To*>(0);
	}

#ifndef NDEBUG
	assert(f == NULL || dynamic_cast<To*>(get_pointer(f)) != NULL);
#endif
	return std::static_pointer_cast<To>(f);
}

namespace reactor{
// 	class Buffer;
// 	class TcpConnection;
// 	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void()> TimerCallBack;

// 	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallBack;
// 	typedef std::function<void(const TcpConnectionPtr&)> CloseCallBack;
// 	typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallBack;
// 	typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallBack;
// 
// 	// the data has been read to (buf, len)
// 	typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallBack;
// 
// 	void defaultConnectionCallBack(const TcpConnectionPtr& conn);
// 	void defaultMessageCallBack(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp recvTime);

}