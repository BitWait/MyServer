#pragma once

#include <vector>
#include <map>

#include "eventloop.h"
struct epoll_event;

namespace reactor{

	class EpollPoller
	{
	public:
		typedef std::vector<Channel*> ChannelList;
		EpollPoller(EventLoop* loop);
		~EpollPoller();

		virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
		virtual void updateChannel(Channel* channel);//维护和更新epollFd_数组

		void assertInLoopThread() const
		{
			ownerLoop_->assertInLoopThread();
		}
		void update(int operation, Channel* channel);
	private:
		void fillActiveChannels(int numEvents,
			ChannelList* activeChannels) const;

		typedef std::vector<struct epoll_event> EventList;

		typedef std::map<int, Channel*> ChannelMap;
		EventList events_;
		ChannelMap channels_;
		EventLoop* ownerLoop_;
		int epollfd_;
	};

}