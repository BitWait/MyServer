#include "timerQueue.h"

#include "../base/logging.h"
#include "eventloop.h"
#include "timer.h"
#include "timerId.h"

#include <functional>

#include <sys/timerfd.h>
#include <unistd.h>

namespace net{
	namespace detail{
		
		int createTimerFd()
		{//创建非阻塞timefd
			int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
			if (timerFd < 0)
			{
				LOG_SYSFATAL << "Failed in timerfd_create";
			}
			return timerFd;
		}

		struct timespec howMuchTimeFromNow(Timestamp when)
		{//现在距离超时还有多久
			int64_t microsecends = when.microSecondsSinceEpoch()
				- Timestamp::now().microSecondsSinceEpoch();
			if (microsecends < 100)
			{
				microsecends = 100;
			}
			struct timespec ts;
			ts.tv_sec = static_cast<time_t>(
				microsecends / Timestamp::kMicroSecondsPerSecond);
			ts.tv_nsec = static_cast<long>(
				(microsecends % Timestamp::kMicroSecondsPerSecond * 1000));

			return ts;
		}

		void readTimerFd(int timeFd, Timestamp now)
		{//处理超时时间，超时后，timefd变为可读,howmany表示超时的次数
			
			uint64_t howMany;//将事件读出来，免得陷入Loop忙碌状态
			size_t n = ::read(timeFd, &howMany, sizeof howMany);
			LOG_TRACE << "TimerQueue::handleRead() " << howMany << " at " << now.toString();
			if (n != sizeof howMany)
			{
				LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
			}
		}

		void resetTimerFd(int timerFd, Timestamp expiration)
		{//重新设置定时器描述符关注的定时事件
			struct itimerspec newValue;
			struct itimerspec oldValue;

			bzero(&newValue, sizeof newValue);
			bzero(&oldValue, sizeof oldValue);

			newValue.it_value = howMuchTimeFromNow(expiration);

			int ret = ::timerfd_settime(timerFd, 0, &newValue, &oldValue);
			if (ret)
			{
				LOG_SYSERR << "timerfd_settime()";
			}
		}
	}
}

using namespace net;
using namespace net::detail;

TimerQueue::TimerQueue(EventLoop* loop)
: loop_(loop),
timerFd_(createTimerFd()),
timerFdChannel_(loop, timerFd_),
timers_(),
callingExpiredTimers_(false)
{
	timerFdChannel_.setReadCallBack(
		std::bind(&TimerQueue::handleRead, this));
	// we are always reading the timerfd, we disarm it with timerfd_settime.
	//将timerfd挂到epollfd上
	timerFdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
	timerFdChannel_.disableAll();
	timerFdChannel_.remove();
	::close(timerFd_);
	// do not remove channel, since we're in EventLoop::dtor();
	for (TimerList::iterator it = timers_.begin();
		it != timers_.end(); ++it)
	{
		delete it->second;
	}
}



TimerId TimerQueue::addTimer(const TimerCallBack& cb,
	Timestamp when,
	double interval)
{//添加新的定时器
	Timer* timer = new Timer(cb, when, interval);
	loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence());
}

TimerId TimerQueue::addTimer(TimerCallBack&& cb,
	Timestamp when,
	double interval)
{
	Timer* timer = new Timer(std::move(cb), when, interval);
	loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
	loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
	loop_->assertInLoopThread();
	bool earliestChanged = insert(timer);//是否将timer插入set的首部

	//如果插入首部，更新timrfd关注的到期时间
	if (earliestChanged)
	{
		resetTimerFd(timerFd_, timer->expiration());   //启动定时器
	}
}

void TimerQueue::cancelInLoop(TimerId timerId)
{//取消定时器
	loop_->assertInLoopThread();
	assert(timers_.size() == activeTimers_.size());
	ActiveTimer timer(timerId.timer_, timerId.sequence_);   //获得索引
	ActiveTimerSet::iterator it = activeTimers_.find(timer);
	if (it != activeTimers_.end())
	{//删除Timers_和activeTimers_中的Timer
		size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
		assert(n == 1); (void)n;
		delete it->first; // FIXME: no delete please
		activeTimers_.erase(it);//删除活动的timer
	}
	else if (callingExpiredTimers_)
	{//将删除的timer加入到取消的timer队列中
		cancelingTimers_.insert(timer);
	}
	assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::handleRead()
{
	loop_->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerFd(timerFd_, now);//读timerFd,防止一直出现可读事件，造成loop忙碌

	std::vector<Entry> expired = getExpired(now);   //获得超时的定时器

	callingExpiredTimers_ = true;//将目前的状态调整为处理超时状态
	cancelingTimers_.clear();		//将取消的定时器清理掉
	//更新完成马上就是重置，重置时依赖已经取消的定时器的条件，所以要将取消的定时器的队列清空
	// safe to callback outside critical section
	for (std::vector<Entry>::iterator it = expired.begin();
		it != expired.end(); ++it)
	{
		it->second->run();//逐个调用超时的定时器的回调
	}
	callingExpiredTimers_ = false;  //退出处理超时定时器额状态

	reset(expired, now);  //把具有重复属性的定时器重新加入定时器队列中
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{//获得当前已经超时的timer
	assert(timers_.size() == activeTimers_.size());
	std::vector<Entry> expired;//存储超时timer的队列
	Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerList::iterator end = timers_.lower_bound(sentry);//返回的一个大于等于now的timer，小于now的都已经超时
	assert(end == timers_.end() || now < end->first);
	std::copy(timers_.begin(), end, back_inserter(expired));//将timer_的begin到上述获得end迭代器元素添加到expired的末尾
	timers_.erase(timers_.begin(), end);//在timer_中删除刚才被添加的元素

	for (std::vector<Entry>::iterator it = expired.begin();
		it != expired.end(); ++it)
	{//在Activetimer_的同步中删除timer
		ActiveTimer timer(it->second, it->second->sequence());
		size_t n = activeTimers_.erase(timer);
		assert(n == 1); (void)n;
	}

	assert(timers_.size() == activeTimers_.size());
	return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{//将具有超时属性的定时器重新加入定时器队列
	Timestamp nextExpire;

	for (std::vector<Entry>::const_iterator it = expired.begin();
		it != expired.end(); ++it)
	{
		ActiveTimer timer(it->second, it->second->sequence());
		if (it->second->repeat()
			&& cancelingTimers_.find(timer) == cancelingTimers_.end())
		{//判断是否具有重复属性并且不在取消的定时器队列中
			it->second->restart(now);//重新设置定时器的到期时间，并且将重新设置后的定时器插入timer_和activeTimer_中
			insert(it->second);
		}
		else
		{
			// FIXME move to a free list
			delete it->second; // FIXME: no delete please
		}
	}

	if (!timers_.empty())
	{//如果目前的队列不为空，获得目前队首的到期时间
		nextExpire = timers_.begin()->second->expiration();
	}

	if (nextExpire.valid())
	{
		resetTimerFd(timerFd_, nextExpire);
	}
}

bool TimerQueue::insert(Timer* timer)
{//将Timer插入到两个同步的TimeQueue中，最关键的一个函数
	loop_->assertInLoopThread();
	assert(timers_.size() == activeTimers_.size());
	bool earliestChanged = false;//判断两个Timer队列的同步
	Timestamp when = timer->expiration();//获得Timer的事件
	TimerList::iterator it = timers_.begin();
	if (it == timers_.end() || when < it->first)
	{//判断是否要将这个timer插入队首，如果是，更新timefd关注的到期事件
		earliestChanged = true;
	}
	{//将Timer中按顺序插入timer_，set是有序集合，默认关键字<排列
		std::pair<TimerList::iterator, bool> result
			= timers_.insert(Entry(when, timer));
		assert(result.second); (void)result;
	}
	{//随意插入进入activeTimer_
		std::pair<ActiveTimerSet::iterator, bool> result
			= activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
		assert(result.second); (void)result;
	}

	assert(timers_.size() == activeTimers_.size());//再次同步两个Timer
	return earliestChanged;
}
