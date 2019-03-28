#ifndef __SIMPLE_TIME_H__
#define __SIMPLE_TIME_H__

#include "../../../tcpserver.h"

class TimeServer
{
public:
	TimeServer(net::EventLoop *loop, const net::InetAddress& addr);


	void start();
private:
	void onConnection(const net::TcpConnectionPtr& conn);

	void onMessage(const net::TcpConnectionPtr& conn, net::Buffer* buf, Timestamp receiveTime);

	net::TcpServer server_;
};

#endif