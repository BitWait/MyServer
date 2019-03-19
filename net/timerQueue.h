#pragma once

#include <set>
#include <vector>

#include "callback.h"
#include "../base/timestamp.h"
#include "channel.h"

namespace net{

	class Timer;
	class TimerId;
	class EventLoop;

	class TimerQueue
	{
	public:
		TimerQueue(EventLoop* loop);
		~TimerQueue();

		TimerId addTimer(const TimerCallBack& cb,
			Timestamp when,
			double interval);

		TimerId addTimer(TimerCallBack&& cb,
			Timestamp when,
			double interval);

		void cancel(TimerId timerId);

	private:
		TimerQueue(const TimerQueue& rhs);
		TimerQueue& operator=(const TimerQueue& rhs);

		typedef pair<Timestamp, Timer*> Entry;                  //到期的时间和指向其的定时器
		typedef set<Entry> TimerList;
		typedef pair<Timer*, int64_t> ActiveTimer;				//定时器和其定时器的序列号
		typedef set<ActiveTimer> ActiveTimerSet;

		void addTimerInLoop(Timer *timer);

		void cancelInLoop(TimerId timerId);

		void handleRead();

		std::vector<Entry> getExpired(Timestamp now);			//返回超时的定时器列表

		void reset(const std::vector<Entry>& expired, Timestamp now);

		bool insert(Timer* timer);								//在两个序列中插入定时器

		EventLoop* loop_;
		const int timerFd_;										//只有一个定时器，防止同时开启多个定时器，占用多余的文件描述符
		Channel timerFdChannel_;								//定时器关心的channel对象
		TimerList timers_;										//定时器集合（有序）

		ActiveTimerSet activeTimers_;							 // 保存正在活动的定时器（无序）
		bool callingExpiredTimers_;								/* atomic *///是否正在处理超时事件

		ActiveTimerSet cancelingTimers_;						//保存的是取消的定时器（无序）
	};

}