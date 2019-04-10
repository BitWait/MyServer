#include "codec.h"
#include "../../../../base/logging.h"
#include "../../../eventloop.h"
#include "../../../../base/singleton.h"
#include "../../../eventloopthreadpool.h"
#include "../../../eventloopthread.h"
#include "../../../tcpclient.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <chrono>

using namespace net;

class ChatClient
{
public:
	ChatClient(EventLoop* loop, const InetAddress& serverAddr)
		: client_(loop, serverAddr, "ChatClient"),
		codec_(std::bind(&ChatClient::onStringMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	{
		client_.setConnectionCallBack(
			std::bind(&ChatClient::onConnection, this, std::placeholders::_1));
		client_.setMessageCallBack(
			std::bind(&LengthHeaderCodec::onMessage, &codec_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		client_.enableRetry();
	}

	void connect()
	{
		client_.connect();
	}

	void disconnect()
	{
		client_.disconnect();
	}

	void write(const std::string& message)
	{
		std::unique_lock<mutex> lock(mutex_);
		if (connection_)
		{
			codec_.send(connection_.get(), message);
		}
	}

private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_INFO << conn->localAddress().toIpPort() << " -> "
			<< conn->peerAddress().toIpPort() << " is "
			<< (conn->connected() ? "UP" : "DOWN");

		std::unique_lock<mutex> lock(mutex_);
		if (conn->connected())
		{
			connection_ = conn;
		}
		else
		{
			connection_.reset();
		}
	}

	void onStringMessage(const TcpConnectionPtr&,
		const string& message,
		Timestamp)
	{
		printf("<<< %s\n", message.c_str());
	}

	TcpClient client_;
	LengthHeaderCodec codec_;
	std::mutex mutex_;
	TcpConnectionPtr connection_;
};

int main(void)
{
	LOG_INFO << "pid = " << getpid();
	EventLoop loop;
	EventLoopThread loopThread;
	InetAddress serverAddr(12345);
	ChatClient client(loopThread.startLoop(), serverAddr);
	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();
	client.connect();
	std::string line;
	while (std::getline(std::cin, line))
	{
		client.write(line);
	}
	client.disconnect();
	std::chrono::milliseconds dura(1000);
	std::this_thread::sleep_for(dura);


}

