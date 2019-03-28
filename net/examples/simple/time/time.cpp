
#include "time.h"

#include "../../../../base/logging.h"
#include "../../../endian.h"
using namespace net;

TimeServer::TimeServer(net::EventLoop *loop, const net::InetAddress& addr)
: server_(loop, addr, "TimeServer")
{
	server_.setConnectionCallback(
		std::bind(&TimeServer::onConnection, this, std::placeholders::_1));
	server_.setMessageCallback(
		std::bind(&TimeServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void TimeServer::start()
{
	server_.start();
}

void TimeServer::onConnection(const net::TcpConnectionPtr& conn)
{
	LOG_INFO << "TimeServer - " << conn->peerAddress().toIpPort() << " -> "
		<< conn->localAddress().toIpPort() << " is "
		<< (conn->connected() ? "UP" : "DOWN");
	if (conn->connected())
	{
		time_t now = ::time(NULL);
		int32_t be32 = sockets::hostToNetwork32(static_cast<int32_t>(now));
		conn->send(&be32, sizeof be32);
		conn->shutdown();
	}
}

void TimeServer::onMessage(const net::TcpConnectionPtr& conn, net::Buffer* buf, Timestamp receiveTime)
{
	string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->name() << " discards " << msg.size()
		<< " bytes received at " << receiveTime.toString();
}
