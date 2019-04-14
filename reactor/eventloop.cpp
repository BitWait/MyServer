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
#include "timerQueue.h"

using namespace reactor;

namespace
{
	__thread EventLoop* t_loopInThisThread = 0;

	const int kPollTimeMs = 1;

	int createEventfd()
	{
		int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		if (evtfd < 0)
		{
			LOG_SYSERR << "Failed in eventfd";
			abort();
		}
		return evtfd;
	}

#pragma GCC diagnostic ignored "-Wold-style-cast"
	class IgnoreSigPipe
	{
	public:
		IgnoreSigPipe()
		{
			::signal(SIGPIPE, SIG_IGN);
			// LOG_TRACE << "Ignore SIGPIPE";
		}
	};
#pragma GCC diagnostic error "-Wold-style-cast"

	IgnoreSigPipe initObj;
}

EventLoop::EventLoop()
: looping_(false),
quit_(false),
poller_(new EpollPoller(this)),
timerQueue_(new TimerQueue(this)),
wakeupFd_(createEventfd()),
wakeupChannel_(new Channel(this, wakeupFd_)),
callingPendingFunctors_(false),
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
		poller_->poll(kPollTimeMs, &activeChannels_);

		for (ChannelList::iterator it = activeChannels_.begin();
			it != activeChannels_.end(); ++it)
		{
			(*it)->handleEvent();
		}
		doPendingFunctors();
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
	///////////////////////////4///////////////////////////////////////////////
	if (!isInLoopThread())
	{
		wakeup();
	}
}

void EventLoop::updateChannel(Channel* channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallBack& cb)
{
	return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallBack& cb)
{
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallBack& cb)
{
	Timestamp time(addTime(Timestamp::now(), interval));
	return timerQueue_->addTimer(cb, time, interval);
}
//////////////////////////////4////////////////////////////////////////////
void EventLoop::runInLoop(const Functor& cb)
{
	if (isInLoopThread())
	{
		cb();
	}
	else
	{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb)
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		pendingFunctors_.push_back(cb);
	}

	if (!isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
	{
		LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
	}
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;

	{
		std::unique_lock<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (size_t i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}
	callingPendingFunctors_ = false;
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
	{
		LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
	}
}