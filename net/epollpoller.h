#pragma once

#include <vector>
#include <map>

#include "../base/timestamp.h"
#include "eventloop.h"

struct epoll_event;

namespace net{
	class EpollPoller 
	{
	public:
		typedef std::vector<Channel*> ChannelList;

		EpollPoller(EventLoop* loop);
		~EpollPoller();

		virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
		virtual void updateChannel(Channel* channel);//维护和更新epollFd_数组
		virtual void removeChannel(Channel* channel);

		virtual bool hasChannel(Channel* channel) const;
		
		void assertInLoopThread() const
		{
			ownerLoop_->assertInLoopThread();
		}


	private:
		static const int kInitEventListSize = 16;

		void fillActiveChannels(int numEvents,
			ChannelList* activeChannels) const;
		void update(int operation, Channel* channel);

		typedef std::vector<struct epoll_event> EventList;

		int epollfd_;
		EventList events_;

		typedef std::map<int, Channel*> ChannelMap;

		ChannelMap channels_;
		EventLoop* ownerLoop_;
	};

	
}