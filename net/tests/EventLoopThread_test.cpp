#include "../eventloopthread.h"
#include "../eventloop.h"
#include "../../base/countdownlatch.h"

#include <stdio.h>
#include <unistd.h>
#include <thread>

#include <chrono>
using namespace net;

void print(EventLoop* p = NULL)
{
	printf("print: pid = %d, tid = %d, loop = %p\n",
		getpid(), std::this_thread::get_id(), p);
}

void quit(EventLoop* p)
{
	print(p);
	p->quit();
}

void runInThread()
{
	printf("runInThread(): pid = %d, tid = %d\n",
		getpid(), std::this_thread::get_id());
}

int main()
{
	print();

	{
		EventLoopThread thr1;  // never start
	}

	{
		// dtor calls quit()
		EventLoopThread thr2;
		EventLoop* loop = thr2.startLoop();
		loop->runInLoop(std::bind(print, loop));
		std::chrono::milliseconds dura(1000);
		std::this_thread::sleep_for(dura);
		loop->runAfter(2, runInThread);
		
	}

	{
		// quit() before dtor
		EventLoopThread thr3;
		EventLoop* loop = thr3.startLoop();
		loop->runInLoop(std::bind(quit, loop));
		std::chrono::milliseconds dura(3000);
		std::this_thread::sleep_for(dura);
	}
}
