#include "../../eventloop.h"
#include "../../tcpserver.h"
#include "../../../base/singleton.h"
#include "../../eventloopthreadpool.h"
using namespace net;

void onMessage(const TcpConnectionPtr& conn,
	Buffer* buf,
	Timestamp receiveTime)
{
	if (buf->findCRLF())
	{
		conn->send("No such user\r\n");
		conn->shutdown();
	}
}


int main(void)
{
	EventLoop loop;
	TcpServer server(&loop, InetAddress(1079), "Finger");
	server.setMessageCallBack(onMessage);
	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();

	server.start();
	loop.loop();
}