#include "accept.h"
#include "../base/logging.h"
#include "eventloop.h"
#include "inetaddress.h"
#include "sockets.h"


#include <functional>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

using namespace net;


Accept::Accept(EventLoop* loop, const InetAddress& listenAddr, bool reusePort)
:loop_(loop), 
acceptSocket_(sockets::createNonblockingOrDie()),
listening_(false),
acceptChannel_(loop, acceptSocket_.fd()),
idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	assert(idleFd_ >= 0);
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reusePort);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallBack(std::bind(&Accept::handleRead, this));
}

Accept::~Accept()
{
	LOG_TRACE << idleFd_;
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(idleFd_);
}

void Accept::listen()
{
	loop_->assertInLoopThread();
	listening_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Accept::handleRead()
{
	loop_->assertInLoopThread();
	InetAddress peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0)
	{
		if (newConnectCallBack_)
		{
			newConnectCallBack_(connfd, peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}
	}
	else
	{
		LOG_SYSERR << "in Acceptor::handleRead";
		if (errno == EMFILE)
		{
			::close(idleFd_);
			idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}