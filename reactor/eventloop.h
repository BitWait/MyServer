#pragma once
#include <thread>
#include <memory>

#include "../base/timestamp.h"

namespace reactor{
	class Channel;
	class EpollPoller;
	class EventLoop
	{
	public:
		EventLoop();

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
	};
}
