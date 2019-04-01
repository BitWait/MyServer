#ifndef __EXAMPLE_ASIO_CHAT_CODEC_H__
#define __EXAMPLE_ASIO_CHAT_CODEC_H__

#include "../../../../base/logging.h"
#include "../../../endian.h"
#include "../../../buffer.h"
#include "../../../tcpconnection.h"

class LengthHeaderCodec
{
public:
	typedef std::function<void(const net::TcpConnectionPtr& ptr,
		const string& msg,
		Timestamp)> StringMsgCallBack;
	explicit LengthHeaderCodec(const StringMsgCallBack& cb)
		:msgCallBack_(cb)
	{

	}

	void onMessage(const net::TcpConnectionPtr& conn, net::Buffer* buf, Timestamp receiveTime)
	{
		while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4
		{
			// FIXME: use Buffer::peekInt32()
			const void* data = buf->peek();
			int32_t be32 = *static_cast<const int32_t*>(data); // SIGBUS
			const int32_t len = net::sockets::networkTohost32(be32);
			if (len > 65536 || len < 0)
			{
				LOG_ERROR << "Invalid length " << len;
				conn->shutdown();  // FIXME: disable reading
				break;
			}
			else if (buf->readableBytes() >= len + kHeaderLen)
			{
				buf->retrieve(kHeaderLen);
				string message(buf->peek(), len);
				msgCallBack_(conn, message, receiveTime);
				buf->retrieve(len);
			}
			else
			{
				break;
			}
		}
	}

	// FIXME: TcpConnectionPtr
	void send(net::TcpConnection* conn,
		const string& message)
	{
		net::Buffer buf;
		buf.append(message.data(), message.size());
		int32_t len = static_cast<int32_t>(message.size());
		int32_t be32 = net::sockets::hostToNetwork32(len);
		buf.prepend(&be32, sizeof be32);
		conn->send(&buf);
	}

private:
	StringMsgCallBack msgCallBack_;
	const static size_t kHeaderLen = sizeof(int32_t);
};

#endif
