#include "../eventloop.h"
#include "../channel.h"
#include <sys/timerfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace reactor;
EventLoop *g_loop;

void timeout()
{
	printf("timeout\n");
	g_loop->quit();
}


int main(void)
{
	EventLoop loop;

	g_loop = &loop;

	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(&loop, timerfd);
	channel.setReadCallBack(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 5;
	::timerfd_settime(timerfd, 0, &howlong, NULL);
	loop.loop();

	::close(timerfd);
	return 0;
}
