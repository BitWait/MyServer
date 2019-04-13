#include "channel.h"
#include "eventloop.h"
#include "../base/logging.h"

#include <poll.h>
using namespace reactor;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd__)
: loop_(loop),
fd_(fd__),
events_(0),
revents_(0),
index_(-1)
{
}

void Channel::update()
{
	loop_->updateChannel(this);
}

//根据revent_的值分别调用不同的用户回调
void Channel::handleEvent()
{
	if (revents_ & POLLNVAL)
	{
		LOG_WARN << "Channel::handle_event() POLLNVAL";
	}

	if (revents_ & (POLLERR | POLLNVAL))
	{
		if (errorCallBack_) errorCallBack_();
	}

	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		//当是侦听socket时，readCallback_指向Acceptor::handleRead
		//当是客户端socket时，调用TcpConnection::handleRead 
		if (readCallBack_) readCallBack_();
	}

	if (revents_ & POLLOUT)
	{
		//如果是连接状态服的socket，则writeCallback_指向Connector::handleWrite()
		if (writeCallBack_) writeCallBack_();
	}
}