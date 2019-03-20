#include "../base/logging.h"
#include "channel.h"
#include "eventloop.h"

#include <sstream>
#include <assert.h>
#include <poll.h>

using namespace net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd__)
: loop_(loop),
fd_(fd__),
events_(0),
revents_(0),
index_(-1),
logHup_(true),
tied_(false),
eventHandling_(false),
addedToLoop_(false)
{
}

void Channel::tie(const std::shared_ptr<void>&obj)
{
	tie_ = obj;
	tied_ = true;
}

void Channel::handleEvent(Timestamp recvTime)
{
	std::shared_ptr<void> guard;
	if (tied_)
	{
		guard = tie_.lock();
		if (guard)
		{
			handleEventWithGuard(recvTime);

		}
	}
	else
	{
		handleEventWithGuard(recvTime);
	}
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
	eventHandling_ = true;
	/*
	POLLIN �����¼�
	POLLPRI�����¼�������ʾ�������ݣ�����tcp socket�Ĵ�������
	POLLRDNORM , ���¼�����ʾ����ͨ���ݿɶ�������
	POLLRDBAND ,�����¼�����ʾ���������ݿɶ���������
	POLLOUT��д�¼�
	POLLWRNORM , д�¼�����ʾ����ͨ���ݿ�д
	POLLWRBAND ,��д�¼�����ʾ���������ݿ�д������   ��������
	POLLRDHUP (since Linux 2.6.17)��Stream socket��һ�˹ر������ӣ�ע����stream socket������֪������raw socket,dgram socket����������д�˹ر������ӣ����Ҫʹ������¼������붨��_GNU_SOURCE �ꡣ����¼����������ж���·�Ƿ����쳣����Ȼ��ͨ�õķ�����ʹ���������ƣ���Ҫʹ������¼�������������ͷ�ļ���
	����#define _GNU_SOURCE
	  ����#include <poll.h>
		POLLERR���������ں����ô�������revents����ʾ�豸��������
		POLLHUP���������ں����ô�������revents����ʾ�豸���������poll������fd��socket����ʾ���socket��û���������Ͻ������ӣ�����˵ֻ������socket()����������û�н���connect��
		POLLNVAL���������ں����ô�������revents����ʾ�Ƿ������ļ�������fdû�д�
		*/
	LOG_TRACE << reventsToString();
	if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
	{
		if (logHup_)
		{
			LOG_WARN << "Channel::handle_event() POLLHUP";
		}
		if (closeCallBack_) closeCallBack_();
	}

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
		if (readCallBack_) readCallBack_(receiveTime);
	}

	if (revents_ & POLLOUT)
	{
		//���������״̬����socket����writeCallback_ָ��Connector::handleWrite()
		if (writeCallBack_) writeCallBack_();
	}
	eventHandling_ = false;
}

void Channel::update()
{
	addedToLoop_ = true;
	loop_->updateChannel(this);
}

void Channel::remove()
{
	assert(isNoneEvent());
	addedToLoop_ = false;
	loop_->removeChannel(this);
}

string Channel::reventsToString() const
{
	std::ostringstream oss;
	oss << fd_ << ": ";
	if (revents_ & POLLIN)
		oss << "IN ";
	if (revents_ & POLLPRI)
		oss << "PRI ";
	if (revents_ & POLLOUT)
		oss << "OUT ";
	if (revents_ & POLLHUP)
		oss << "HUP ";
	if (revents_ & POLLRDHUP)
		oss << "RDHUP ";
	if (revents_ & POLLERR)
		oss << "ERR ";
	if (revents_ & POLLNVAL)
		oss << "NVAL ";

	return oss.str().c_str();
}
