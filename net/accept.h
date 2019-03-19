#pragma once

#include <functional>

#include "channel.h"
#include "sockets.h"

namespace net{
	class InetAddress;
	class EventLoop;
	
	class Accept
	{
	public:
		typedef std::function<void(int socketFd, const InetAddress& addr)> NewConnectCallBack;

		Accept(EventLoop* loop, const InetAddress& listenAddr, bool reUsePort);
		~Accept();
		void setNewConnectCallBack(const NewConnectCallBack &cb)
		{
			newConnectCallBack_ = cb;
		}
		bool listening()const { return listening_; }
		void listen();

	private:
		void handleRead();

		EventLoop*            loop_;
		Socket                acceptSocket_;
		bool                  listening_;
		int                   idleFd_;
		NewConnectCallBack    newConnectCallBack_;
		Channel               acceptChannel_;
	};

}