#include "time.h"

#include "../../../../base/logging.h"
#include "../../../eventloop.h"
#include "../../../../base/singleton.h"
#include "../../../eventloopthreadpool.h"

#include <unistd.h>


using namespace net;

int main()
{
	LOG_INFO << "pid = " << getpid();
	EventLoop loop;
	InetAddress listenAddr(2037);
	TimeServer server(&loop, listenAddr);

	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();
	server.start();
	loop.loop();
}