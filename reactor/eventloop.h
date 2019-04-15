#pragma once
#include <thread>
#include <memory>
#include <functional>
#include <mutex>

#include "../base/timestamp.h"
#include "timerId.h"
#include "callback.h"


namespace reactor{
	class Channel;
	class EpollPoller;
	class TimerQueue;
	class EventLoop
	{
	public:
		
		EventLoop();
		~EventLoop();

		void loop();
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
		void quit();//2
		void updateChannel(Channel* channel);
		//+++++++++++++++++++++++3++++++++++++++++++++++++++++++++++++++++//
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
		//+++++++++++++++++++++++3++++++++++++++++++++++++++++++++++++++++//

		//+++++++++++++++++++++++4++++++++++++++++++++++++++++++++++++++++//
		typedef std::function<void()> Functor;
		void runInLoop(const Functor& cb);
		void wakeup();
		void queueInLoop(const Functor& cb);
		//+++++++++++++++++++++++4++++++++++++++++++++++++++++++++++++++++//
	private:
		
		void abortNotInLoopThread();
		
		bool                                looping_; /* atomic */
		const std::thread::id               threadId_;

		//+++++++++++++++++2++++++++++++++++++//
		typedef std::vector<Channel*> ChannelList;
		bool                                quit_; /* atomic and shared between threads, okay on x86, I guess. */
		std::shared_ptr<EpollPoller>        poller_;
		ChannelList                         activeChannels_;
		//+++++++++++++++++2++++++++++++++++++//

		//+++++++++++++++++3++++++++++++++++++//
		std::shared_ptr<TimerQueue>         timerQueue_;
	private:
		//+++++++++++++++++4++++++++++++++++++//
		void handleRead();  // waked up
		void doPendingFunctors();
		bool                                callingPendingFunctors_; /* atomic */
		int wakeupFd_;
		// unlike in TimerQueue, which is an internal class,
		// we don't expose Channel to client.
		std::shared_ptr<Channel>            wakeupChannel_;
		std::mutex                          mutex_;
		std::vector<Functor>                pendingFunctors_; // Guarded by mutex_
		//+++++++++++++++++4++++++++++++++++++//
		
	};
}
