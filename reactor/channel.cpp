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

//����revent_��ֵ�ֱ���ò�ͬ���û��ص�
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
		//��������socketʱ��readCallback_ָ��Acceptor::handleRead
		//���ǿͻ���socketʱ������TcpConnection::handleRead 
		if (readCallBack_) readCallBack_();
	}

	if (revents_ & POLLOUT)
	{
		//���������״̬����socket����writeCallback_ָ��Connector::handleWrite()
		if (writeCallBack_) writeCallBack_();
	}
}