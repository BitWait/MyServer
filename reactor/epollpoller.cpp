#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "epollpoller.h"
#include "../base/logging.h"
#include "channel.h"

using namespace reactor;

static_assert(EPOLLIN == POLLIN, "EPOLLIN == POLLIN");
static_assert(EPOLLPRI == POLLPRI, "EPOLLPRI == POLLPRI");
static_assert(EPOLLOUT == POLLOUT, "EPOLLOUT == POLLOUT");
static_assert(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP == POLLRDHUP");
static_assert(EPOLLERR == POLLERR, "EPOLLERR == POLLERR");
static_assert(EPOLLHUP == POLLHUP, "EPOLLHUP == POLLHUP");

namespace
{
	const int kNew = -1;
	const int kAdded = 1;
	const int kDeleted = 2;
}

EpollPoller::EpollPoller(EventLoop* loop)
:epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
ownerLoop_(loop),
events_(kInitEventListSize)
{
	if (epollfd_ < 0)
	{
		LOG_SYSFATAL << "EPollPoller::EPollPoller";
	}
}

EpollPoller::~EpollPoller()
{
	::close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = ::epoll_wait(epollfd_,
		&*events_.begin(),
		static_cast<int>(events_.size()),
		timeoutMs);
	int savedErrno = errno;
	Timestamp now(Timestamp::now());
	if (numEvents > 0)
	{
		LOG_TRACE << numEvents << " events happended";
		fillActiveChannels(numEvents, activeChannels);
	}
	else if (numEvents == 0)
	{
		//LOG_TRACE << " nothing happended";
	}
	else
	{
		// error happens, log uncommon ones
		if (savedErrno != EINTR)
		{
			errno = savedErrno;
			LOG_SYSERR << "EPollPoller::poll()";
		}
	}
	return now;
}

void EpollPoller::fillActiveChannels(int numEvents,
	ChannelList* activeChannels) const
{
	assert(static_cast<size_t>(numEvents) <= events_.size());
	for (int i = 0; i < numEvents; ++i)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
#ifndef NDEBUG
		int fd = channel->fd();
		ChannelMap::const_iterator it = channels_.find(fd);
		assert(it != channels_.end());
		assert(it->second == channel);
#endif
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void EpollPoller::updateChannel(Channel* channel)
{
	assertInLoopThread();
	LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
	const int index = channel->index();
	if (index == kNew || index == kDeleted)
	{
		// a new one, add with EPOLL_CTL_ADD
		int fd = channel->fd();
		if (index == kNew)
		{
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		}
		else // index == kDeleted
		{
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
		}
		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		// update existing one with EPOLL_CTL_MOD/DEL
		int fd = channel->fd();
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		assert(index == kAdded);
		if (channel->isNoneEvent())
		{
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		}
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void EpollPoller::update(int operation, Channel* channel)
{
	struct epoll_event event;
	bzero(&event, sizeof event);
	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();
	if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
	{
		if (operation == EPOLL_CTL_DEL)
		{
			LOG_SYSERR << "epoll_ctl op=" << operation << " fd=" << fd;
		}
		else
		{
			//FIXME�� epoll_ctlִ��ʧ������ᵼ�³����˳�
			LOG_SYSFATAL << "epoll_ctl op=" << operation << " fd=" << fd;
		}
	}
}
