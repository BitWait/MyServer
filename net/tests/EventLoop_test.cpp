#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "../eventloop.h"

using namespace net;
EventLoop* g_loop;

void callback()
{
	printf("callback(): pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());
	EventLoop anotherLoop;
}

void threadFunc()
{
	printf("threadFunc(): pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());

	assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
	EventLoop loop;
	assert(EventLoop::getEventLoopOfCurrentThread() == &loop);
	loop.runAfter(1.0, callback);
	loop.loop();
}

int main(void)
{
	printf("main(): pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());
	assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
	EventLoop loop;
	assert(EventLoop::getEventLoopOfCurrentThread() == &loop);

	std::thread thread(threadFunc);

	loop.loop();

}