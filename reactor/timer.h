#pragma once

#include <atomic>
#include <stdint.h>
#include "../base/timestamp.h"
#include "callback.h"

namespace reactor{

	class Timer
	{
	public:
		Timer(const TimerCallBack& cb, Timestamp when, double interval)
			: callBack_(cb),
			expiration_(when),
			interval_(interval),
			repeat_(interval > 0.0),
			sequence_(++s_numCreated_)
		{ }


		Timer(TimerCallBack&& cb, Timestamp when, double interval)
			: callBack_(std::move(cb)),
			expiration_(when),
			interval_(interval),
			repeat_(interval > 0.0),
			sequence_(++s_numCreated_)
		{ }

		void run() const
		{
			callBack_();
		}

		Timestamp expiration() const  { return expiration_; }
		bool repeat() const { return repeat_; }
		int64_t sequence() const { return sequence_; }

		void restart(Timestamp now);

		static int64_t numCreated() { return s_numCreated_; }
	private:
		Timer(const Timer& rhs) = delete;
		Timer& operator=(const Timer& rhs) = delete;

		const TimerCallBack                   callBack_;     //�ص�����
		Timestamp                             expiration_;   //timeout
		const double                          interval_;     //������ظ����ԣ���ʱ��ʱ����
		const bool                            repeat_;       //�ظ�
		const int64_t						  sequence_;     //���
		static std::atomic<int64_t>           s_numCreated_; //��ʱ������
	};

}