#include "../../eventloop.h"
#include "../../tcpserver.h"
#include "../../../base/singleton.h"
#include "../../eventloopthreadpool.h"
using namespace net;

void onConnection(const TcpConnectionPtr& conn)
{
	if (conn->connected())
	{
		conn->shutdown();
	}
}


int main(void)
{
	EventLoop loop;
	TcpServer server(&loop, InetAddress(1079), "Finger");
	server.setConnectionCallBack(onConnection);
	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();

	server.start();
	loop.loop();
}