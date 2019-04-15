#include "timerQueue.h"

#include "../base/logging.h"
#include "eventloop.h"
#include "timer.h"
#include "timerId.h"

#include <functional>

#include <sys/timerfd.h>
#include <unistd.h>

namespace reactor{
	namespace detail{
		
		int createTimerFd()
		{//����������timefd
			int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
			if (timerFd < 0)
			{
				LOG_SYSFATAL << "Failed in timerfd_create";
			}
			return timerFd;
		}

		struct timespec howMuchTimeFromNow(Timestamp when)
		{//���ھ��볬ʱ���ж��
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
		{//����ʱʱ�䣬��ʱ��timefd��Ϊ�ɶ�,howmany��ʾ��ʱ�Ĵ���
			
			uint64_t howMany;//���¼����������������Loopæµ״̬
			size_t n = ::read(timeFd, &howMany, sizeof howMany);
			LOG_TRACE << "TimerQueue::handleRead() " << howMany << " at " << now.toString();
			if (n != sizeof howMany)
			{
				LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
			}
		}

		void resetTimerFd(int timerFd, Timestamp expiration)
		{//�������ö�ʱ����������ע�Ķ�ʱ�¼�
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

using namespace reactor;
using namespace reactor::detail;

TimerQueue::TimerQueue(EventLoop* loop)
: loop_(loop),
timerFd_(createTimerFd()),
timerFdChannel_(loop, timerFd_),
timers_()
{
	timerFdChannel_.setReadCallBack(
		std::bind(&TimerQueue::handleRead, this));
	// we are always reading the timerfd, we disarm it with timerfd_settime.
	//��timerfd�ҵ�epollfd��
	timerFdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
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
{//����µĶ�ʱ��
	Timer* timer = new Timer(cb, when, interval);
	///////////////////////////////5///////////////////////////////////////////
	loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence());
// 	loop_->assertInLoopThread();
// 	bool earliestChanged = insert(timer
// 	if (earliestChanged)
// 	{
// 		resetTimerFd(timerFd_, timer->expiration());
// 	}
// 	return TimerId(timer, timer->sequence());
}


void TimerQueue::handleRead()
{
	loop_->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerFd(timerFd_, now);//��timerFd,��ֹһֱ���ֿɶ��¼������loopæµ

	std::vector<Entry> expired = getExpired(now);   //��ó�ʱ�Ķ�ʱ��

	//����������Ͼ������ã�����ʱ�����Ѿ�ȡ���Ķ�ʱ��������������Ҫ��ȡ���Ķ�ʱ���Ķ������
	// safe to callback outside critical section
	for (std::vector<Entry>::iterator it = expired.begin();
		it != expired.end(); ++it)
	{
		it->second->run();//������ó�ʱ�Ķ�ʱ���Ļص�
	}

	reset(expired, now);  //�Ѿ����ظ����ԵĶ�ʱ�����¼��붨ʱ��������
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{//��õ�ǰ�Ѿ���ʱ��timer
	std::vector<Entry> expired;//�洢��ʱtimer�Ķ���
	Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerList::iterator end = timers_.lower_bound(sentry);//���ص�һ�����ڵ���now��timer��С��now�Ķ��Ѿ���ʱ
	assert(end == timers_.end() || now < end->first);
	std::copy(timers_.begin(), end, back_inserter(expired));//��timer_��begin���������end������Ԫ����ӵ�expired��ĩβ
	timers_.erase(timers_.begin(), end);//��timer_��ɾ���ղű���ӵ�Ԫ��

	return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{//�����г�ʱ���ԵĶ�ʱ�����¼��붨ʱ������
	Timestamp nextExpire;

	for (std::vector<Entry>::const_iterator it = expired.begin();
		it != expired.end(); ++it)
	{
		if (it->second->repeat())
		{//�ж��Ƿ�����ظ����Բ��Ҳ���ȡ���Ķ�ʱ��������
			it->second->restart(now);//�������ö�ʱ���ĵ���ʱ�䣬���ҽ��������ú�Ķ�ʱ������timer_��activeTimer_��
			insert(it->second);
		}
		else
		{
			// FIXME move to a free list
			delete it->second; // FIXME: no delete please
		}
	}

	if (!timers_.empty())
	{//���Ŀǰ�Ķ��в�Ϊ�գ����Ŀǰ���׵ĵ���ʱ��
		nextExpire = timers_.begin()->second->expiration();
	}

	if (nextExpire.valid())
	{
		resetTimerFd(timerFd_, nextExpire);
	}
}

bool TimerQueue::insert(Timer* timer)
{//��Timer���뵽����ͬ����TimeQueue�У���ؼ���һ������
	loop_->assertInLoopThread();
	bool earliestChanged = false;//�ж�����Timer���е�ͬ��
	Timestamp when = timer->expiration();//���Timer���¼�
	TimerList::iterator it = timers_.begin();
	if (it == timers_.end() || when < it->first)
	{//�ж��Ƿ�Ҫ�����timer������ף�����ǣ�����timefd��ע�ĵ����¼�
		earliestChanged = true;
	}
	{//��Timer�а�˳�����timer_��set�����򼯺ϣ�Ĭ�Ϲؼ���<����
		std::pair<TimerList::iterator, bool> result
			= timers_.insert(Entry(when, timer));
		assert(result.second); (void)result;
	}
	return earliestChanged;
}

//////////////////////////////////////5////////////////////////////////////
void TimerQueue::addTimerInLoop(Timer* timer)
{
	loop_->assertInLoopThread();
	bool earliestChanged = insert(timer);//�Ƿ�timer����set���ײ�

	//��������ײ�������timrfd��ע�ĵ���ʱ��
	if (earliestChanged)
	{
		resetTimerFd(timerFd_, timer->expiration());   //������ʱ��
	}
}