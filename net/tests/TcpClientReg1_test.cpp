// TcpClient::stop() called in the same iteration of IO event

#include "../../base/logging.h"
#include "../eventloop.h"
#include "../tcpclient.h"
#include <thread>
using namespace net;

TcpClient* g_client;

void timeout()
{
	LOG_INFO << "timeout";
	g_client->stop();
}

int main(int argc, char* argv[])
{
	EventLoop loop;
	InetAddress serverAddr("127.0.0.1", 2); // no such server
	TcpClient client(&loop, serverAddr, "TcpClient");
	g_client = &client;
	loop.runAfter(0.0, timeout);
	loop.runAfter(1.0, std::bind(&EventLoop::quit, &loop));
	client.connect();

	loop.loop();
}
