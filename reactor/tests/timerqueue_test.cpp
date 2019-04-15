// copied from muduo/net/tests/TimerQueue_unittest.cc

#include "../eventloop.h"
#include "../../base/timestamp.h"
#include <thread>
#include <unistd.h>

#include <stdio.h>
using namespace reactor;
int cnt = 0;
reactor::EventLoop* g_loop;

void printTid()
{
	printf("pid = %d, tid = %d\n", getpid(),std::this_thread::get_id);
	printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char* msg)
{
	printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
	if (++cnt == 20)
	{
		g_loop->quit();
	}
}

void threadFunc(void)
{
	g_loop->runAfter(1, printTid);
}
int main()
{
	printTid();
	reactor::EventLoop loop;
	g_loop = &loop;

	std::thread thread(threadFunc);
// 	print("main");
// 	loop.runAfter(1, std::bind(print, "once1"));
// 	loop.runAfter(1.5, std::bind(print, "once1.5"));
// 	loop.runAfter(2.5, std::bind(print, "once2.5"));
// 	loop.runAfter(3.5, std::bind(print, "once3.5"));
// 	loop.runEvery(2, std::bind(print, "every2"));
// 	loop.runEvery(3, std::bind(print, "every3"));

	loop.loop();
	thread.join();
	print("main loop exits");
	sleep(1);
}
