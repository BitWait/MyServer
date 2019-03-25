#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>

#include "../base/timestamp.h"
#include "callback.h"
#include "timerId.h"
#include <iostream>

namespace net{

	class Channel;
	class EpollPoller;
	class TimerQueue;

	class EventLoop
	{
	public:
		typedef std::function<void()> Functor;
		EventLoop();
		~EventLoop();

		void loop();
		void quit();

		Timestamp pollReturnTime() const { return pollReturnTime_; }

		int64_t iteration() const { return iteration_; }

		void runInLoop(const Functor& cb);
		/// Queues callback in the loop thread.
		/// Runs after finish pooling.
		/// Safe to call from other threads.
		void queueInLoop(const Functor& cb);

		TimerId runAt(const Timestamp& time, const TimerCallBack& cb);
		///
		/// Runs callback after @c delay seconds.
		/// Safe to call from other threads.
		///
		TimerId runAfter(double delay, const TimerCallBack& cb);
		///
		/// Runs callback every @c interval seconds.
		/// Safe to call from other threads.
		///
		TimerId runEvery(double interval, const TimerCallBack& cb);
		///
		/// Cancels the timer.
		/// Safe to call from other threads.
		///
		void cancel(TimerId timerId);

		TimerId runAt(const Timestamp& time, TimerCallBack&& cb);
		TimerId runAfter(double delay, TimerCallBack&& cb);
		TimerId runEvery(double interval, TimerCallBack&& cb);

		void setFrameFunctor(const Functor& cb);
		void wakeup();
		void updateChannel(Channel* channel);
		void removeChannel(Channel* channel);
		bool hasChannel(Channel* channel);

		void assertInLoopThread()
		{
			if (!isInLoopThread())
			{
				abortNotInLoopThread();
			}
		}
		bool isInLoopThread() const 
		{ 
			return threadId_ == std::this_thread::get_id(); 
		}
		 bool callingPendingFunctors() const { return callingPendingFunctors_; }
		bool eventHandling() const { return eventHandling_; }

		static EventLoop* getEventLoopOfCurrentThread();

		const std::thread::id getThreadID() const
		{
			return threadId_;
		}

	private:

		void abortNotInLoopThread();
		void handleRead();  // waked up
		void doPendingFunctors();

		void printActiveChannels() const; // DEBUG

	private:
		typedef std::vector<Channel*> ChannelList;

		bool                                looping_; /* atomic */
		bool                                quit_; /* atomic and shared between threads, okay on x86, I guess. */
		bool                                eventHandling_; /* atomic */
		bool                                callingPendingFunctors_; /* atomic */
		int64_t                             iteration_;
		const std::thread::id               threadId_;
		Timestamp                           pollReturnTime_;
		std::shared_ptr<EpollPoller>        poller_;
		std::shared_ptr<TimerQueue>         timerQueue_;


		int wakeupFd_;
		// unlike in TimerQueue, which is an internal class,
		// we don't expose Channel to client.
		std::shared_ptr<Channel>            wakeupChannel_;

		// scratch variables
		ChannelList                         activeChannels_;
		Channel*                            currentActiveChannel_;

		std::mutex                          mutex_;
		std::vector<Functor>                pendingFunctors_; // Guarded by mutex_

		Functor                             frameFunctor_;
	};


}