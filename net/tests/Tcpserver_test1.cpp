#include "../tcpserver.h"
#include "../eventloop.h"
using namespace net;


int main(void)
{
	EventLoop loop;
	TcpServer server(&loop, InetAddress(1079), "Finger");
	server.start();
	loop.loop();
}