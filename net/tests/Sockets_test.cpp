#include "../sockets.h"
#include "../inetaddress.h"
#include <unistd.h>
#include "../../base/logging.h"
#include <stdio.h>
#include "../accept.h"
#include "../eventloop.h"
#include <thread>

//using net::InetAddress;
using namespace net;

void newConnection(int sockFd, const InetAddress& peerAddr)
{
	printf("newConnection(): accept a new connection from %s\n", peerAddr.toIpPort().c_str());
	::write(sockFd, "How are you\n", 13);
	sockets::close(sockFd);
}

int main(void)
{
	printf("pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());

	InetAddress addr(9981);

	EventLoop loop;

	Accept accept(&loop, addr, true);
	
	assert(!accept.listening());
	
	accept.setNewConnectCallBack(newConnection);

	accept.listen();


	loop.loop();
}