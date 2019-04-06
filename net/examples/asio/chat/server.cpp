#include "codec.h"
#include "../../../../base/logging.h"
#include "../../../eventloop.h"
#include "../../../tcpserver.h"
#include "../../../../base/singleton.h"
#include "../../../eventloopthreadpool.h"

#include <set>
#include <stdio.h>
#include <unistd.h>

using namespace net;

class ChatServer
{
public:
	ChatServer(EventLoop* loop,
		const InetAddress& listenAddr)
		: server_(loop, listenAddr, "ChatServer"),
		codec_(std::bind(&ChatServer::onStringMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	{
		server_.setConnectionCallBack(
			std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
		server_.setMessageCallBack(
			std::bind(&LengthHeaderCodec::onMessage, &codec_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void start()
	{
		server_.start();
	}

private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_INFO << conn->localAddress().toIpPort() << " -> "
			<< conn->peerAddress().toIpPort() << " is "
			<< (conn->connected() ? "UP" : "DOWN");

		if (conn->connected())
		{
			connections_.insert(conn);
		}
		else
		{
			connections_.erase(conn);
		}
	}

	void onStringMessage(const TcpConnectionPtr&,
		const string& message,
		Timestamp)
	{
		for (ConnectionList::iterator it = connections_.begin();
			it != connections_.end();
			++it)
		{
			codec_.send((*it).get(), message);
		}
	}

	typedef std::set<TcpConnectionPtr> ConnectionList;
	TcpServer server_;
	LengthHeaderCodec codec_;
	ConnectionList connections_;
};

int main(void)
{
	LOG_INFO << "pid = " << getpid();

	EventLoop loop;
	InetAddress serverAddr(12345);
	ChatServer server(&loop, serverAddr);
	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();
	server.start();
	loop.loop();
}