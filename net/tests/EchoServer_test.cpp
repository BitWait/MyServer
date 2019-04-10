#include "../eventloop.h"
#include "../../base/logging.h"
#include "../tcpserver.h"
#include "../inetaddress.h"
#include "../../base/singleton.h"
#include "../eventloopthreadpool.h"

#include <thread>
#include <unistd.h>
#include <utility>
#include <stdio.h>

using namespace net;

class EchoServer
{
public:
	EchoServer(EventLoop* loop, InetAddress &lisrenAddr)
		:loop_(loop),
		server_(loop, lisrenAddr, "EchoServer")
	{
		server_.setConnectionCallBack(std::bind(&EchoServer::newConnection, this, std::placeholders::_1));
		server_.setMessageCallBack(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	void newConnection(const TcpConnectionPtr& conn)
	{
		LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
			<< conn->localAddress().toIpPort() << " is "
			<< (conn->connected() ? "UP" : "DOWN");
		LOG_INFO << conn->getTcpInfoString();

		conn->send("hello");
	}

	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
	{
		string msg(buf->retrieveAllAsString());
		LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
		if (msg == "exit\n")
		{
			conn->send("bye\n");
			conn->shutdown();
		}
		if (msg == "quit\n")
		{
			loop_->quit();
		}
		conn->send(msg);
	}

	void start(void)
	{
		server_.start();
	}
private:
	EventLoop* loop_;
	TcpServer server_;
};



int main(void)
{
	printf("pid = %d, tid = %lld", getpid(), std::this_thread::get_id());
	//LOG_INFO << "pid = " << getpid() << std::this_thread::get_id();
	LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);
	EventLoop loop;
	InetAddress listenAddr(20000);
	LOG_INFO << listenAddr.toIpPort();
	EchoServer server(&loop, listenAddr);

	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();

	server.start();

	loop.loop();
}