#include "../../base/logging.h"
#include "../eventloop.h"
#include "../channel.h"

#include <functional>
#include <map>

#include <stdio.h>
#include <unistd.h>
#include <sys/timerfd.h>

using namespace net;

void print(const char* msg)
{
	static std::map<const char*, Timestamp> lasts;
	Timestamp& last = lasts[msg];
	Timestamp now = Timestamp::now();
	printf("%s tid %d %s delay %f\n", now.toString().c_str(), std::this_thread::get_id(),
		msg, timeDifference(now, last));
	last = now;
}

int main(void)
{
	printf("main(): pid = %d, tid = %d\n", getpid(), std::this_thread::get_id());
	EventLoop loop;
	loop.runEvery(1, std::bind(print, "EventLoop::runEvery"));
	loop.loop();
}