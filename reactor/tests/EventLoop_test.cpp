#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <thread>
#include "../eventloop.h"


#define test1 0

using namespace reactor;
EventLoop* g_loop;

// void callback()
// {
// 	printf("callback(): pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());
// 	EventLoop anotherLoop;
// }

void threadFunc()
{
	printf("threadFunc(): pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());
#if test1
	EventLoop loop;
#else if
	g_loop->loop();
#endif
}

int main(void)
{
	printf("main(): pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());
	EventLoop loop;
	std::thread thread(threadFunc);
#if test1
	loop.loop();
#else if
	g_loop = &loop;
#endif
	thread.join();
	//EventLoop loop2;
}