#pragma once

#include <set>
#include <vector>

#include "callback.h"
#include "../base/timestamp.h"
#include "channel.h"

namespace reactor{

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
		void handleRead();
		void addTimerInLoop(Timer *timer);

	private:
		TimerQueue(const TimerQueue& rhs);
		TimerQueue& operator=(const TimerQueue& rhs);

		typedef pair<Timestamp, Timer*> Entry;                  //���ڵ�ʱ���ָ����Ķ�ʱ��
		typedef set<Entry> TimerList;

		
		std::vector<Entry> getExpired(Timestamp now);			//���س�ʱ�Ķ�ʱ���б�

		void reset(const std::vector<Entry>& expired, Timestamp now);

		bool insert(Timer* timer);								//�����������в��붨ʱ��

		EventLoop* loop_;
		const int timerFd_;										//ֻ��һ����ʱ������ֹͬʱ���������ʱ����ռ�ö�����ļ�������
		Channel timerFdChannel_;								//��ʱ�����ĵ�channel����
		TimerList timers_;										//��ʱ�����ϣ�����
	};

}