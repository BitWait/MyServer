#include "../eventloop.h"
#include "../eventloopthreadpool.h"
#include "../../base/singleton.h"
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

using namespace net;

void print(EventLoop* p = NULL)
{
	printf("main(): pid = %d, tid = %d, loop = %p\n",
		getpid(),std::this_thread::get_id(), p);
}

void init(EventLoop* p)
{
	printf("init(): pid = %d, tid = %d, loop = %p\n",
		getpid(), std::this_thread::get_id(), p);
}

int main()
{
	print();

	EventLoop loop;
	loop.runAfter(11, std::bind(&EventLoop::quit, &loop));

	{
		printf("Single thread %p:\n", &loop);
		EventLoopThreadPool model;
		model.Init(&loop, 0);
		model.start(init);
		assert(model.getNextLoop() == &loop);
		assert(model.getNextLoop() == &loop);
		assert(model.getNextLoop() == &loop);
	}

	{
		printf("Another thread:\n");
		EventLoopThreadPool model;
		model.Init(&loop, 1);
		model.start(init);
		EventLoop* nextLoop = model.getNextLoop();
		nextLoop->runAfter(2, std::bind(print, nextLoop));
		assert(nextLoop != &loop);
		assert(nextLoop == model.getNextLoop());
		assert(nextLoop == model.getNextLoop());
		::sleep(3);
	}

	{
		printf("Three threads:\n");
		EventLoopThreadPool model;
		model.Init(&loop, 3);
		model.start(init);
		EventLoop* nextLoop = model.getNextLoop();
		nextLoop->runInLoop(std::bind(print, nextLoop));
		assert(nextLoop != &loop);
		assert(nextLoop != model.getNextLoop());
		assert(nextLoop != model.getNextLoop());
		assert(nextLoop == model.getNextLoop());
	}

	loop.loop();
}



