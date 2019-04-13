#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <poll.h>

#include "eventloop.h"
#include "../base/logging.h"
#include "channel.h"
#include "epollpoller.h"

using namespace reactor;
__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop()
: looping_(false),
quit_(false),
threadId_(std::this_thread::get_id())
{
	if (t_loopInThisThread)
	{
		LOG_FATAL << "Another EventLoop  exists in this thread ";
	}
	else
	{
		t_loopInThisThread = this;
	}
}

void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread();
	looping_ = true;
	quit_ = false;
	LOG_TRACE << "EventLoop " << this << " start looping";
	while (!quit_)
	{
		activeChannels_.clear();
		poller_->poll(1, &activeChannels_);

		for (ChannelList::iterator it = activeChannels_.begin();
			it != activeChannels_.end(); ++it)
		{
			(*it)->handleEvent();
		}
	}
	LOG_TRACE << "EventLoop " << this << " stop looping";
	looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
	std::stringstream ss;
	ss << "threadid_ = " << threadId_ << " this_thread::get_id() = " << std::this_thread::get_id();
	LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << ss.str();
}

void EventLoop::quit()
{
	quit_ = true;

}

void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}