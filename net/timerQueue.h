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

		typedef pair<Timestamp, Timer*> Entry;                  //���ڵ�ʱ���ָ����Ķ�ʱ��
		typedef set<Entry> TimerList;
		typedef pair<Timer*, int64_t> ActiveTimer;				//��ʱ�����䶨ʱ�������к�
		typedef set<ActiveTimer> ActiveTimerSet;

		void addTimerInLoop(Timer *timer);

		void cancelInLoop(TimerId timerId);

		void handleRead();

		std::vector<Entry> getExpired(Timestamp now);			//���س�ʱ�Ķ�ʱ���б�

		void reset(const std::vector<Entry>& expired, Timestamp now);

		bool insert(Timer* timer);								//�����������в��붨ʱ��

		EventLoop* loop_;
		const int timerFd_;										//ֻ��һ����ʱ������ֹͬʱ���������ʱ����ռ�ö�����ļ�������
		Channel timerFdChannel_;								//��ʱ�����ĵ�channel����
		TimerList timers_;										//��ʱ�����ϣ�����

		ActiveTimerSet activeTimers_;							 // �������ڻ�Ķ�ʱ��������
		bool callingExpiredTimers_;								/* atomic *///�Ƿ����ڴ���ʱ�¼�

		ActiveTimerSet cancelingTimers_;						//�������ȡ���Ķ�ʱ��������
	};

}