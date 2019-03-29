#pragma once

#include <functional>
#include <memory>
#include "inetaddress.h"

namespace net{
	class EventLoop;
	class Channel;

	class Connector : public std::enable_shared_from_this<Connector>
	{
	public:
		typedef std::function<void(int sockfd)> NewConnectionCallBack;
		Connector(EventLoop* loop, const InetAddress& addr);
		~Connector();

		void setNewConnectionCallBack(const NewConnectionCallBack& cb)
		{
			newConnectionCallBack_ = cb;
		}

		void start();  // can be called in any thread
		void restart();  // must be called in loop thread
		void stop();  // can be called in any thread

		const InetAddress& serverAddress() const { return serverAddr_; }
	private:
		enum States { kDisconnected, kConnecting, kConnected };
		static const int kMaxRetryDelayMs = 30 * 1000;
		static const int kInitRetryDelayMs = 500;

		void setState(States s) { state_ = s; }
		void startInLoop();
		void stopInLoop();
		void connect();
		void connecting(int sockfd);
		void handleWrite();
		void handleError();
		void retry(int sockfd);
		int removeAndResetChannel();
		void resetChannel();


		EventLoop* loop_;
		InetAddress serverAddr_;
		bool connect_;
		States state_;
		std::shared_ptr<Channel> channel_;
		NewConnectionCallBack newConnectionCallBack_;
		int retryDelayMs_;
	};

	
}