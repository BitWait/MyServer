#include "../../eventloop.h"
#include "../../tcpserver.h"
#include "../../../base/singleton.h"
#include "../../eventloopthreadpool.h"
using namespace net;

int main(void)
{
	EventLoop loop;
	TcpServer server(&loop, InetAddress(1079), "Finger");

	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();

	server.start();
	loop.loop();
}
