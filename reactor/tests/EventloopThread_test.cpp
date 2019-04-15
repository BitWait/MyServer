#include "../eventloop.h"
#include "../eventloopthread.h"
#include <stdio.h>
#include <thread>
#include<unistd.h>
using namespace reactor;

void runInThread()
{
	printf("runInThread(): pid = %d, tid = %d\n",
		getpid(), std::this_thread::get_id());
}

int main()
{
	printf("main(): pid = %d, tid = %d\n",
		getpid(), std::this_thread::get_id());

	EventLoopThread loopThread;
	EventLoop* loop = loopThread.startLoop();
	loop->runInLoop(runInThread);
	sleep(1);
	loop->runAfter(2, runInThread);
	sleep(3);
	loop->quit();

	printf("exit main().\n");
}
