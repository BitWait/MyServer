#pragma once

#include <mutex>
#include <string>
#include "tcpconnection.h"

namespace net{
	class Connector;
	typedef std::shared_ptr<Connector> ConnectorPtr;

	class TcpClient
	{
	public:
		TcpClient(EventLoop* loop,
			const InetAddress& serverAddr,
			const string& nameArg);
		~TcpClient();
		void connect();
		void disconnect();
		void stop();

		TcpConnectionPtr connection() const
		{
			std::unique_lock<std::mutex> lock(mutex_);
			return connection_;
		}

		EventLoop* getLoop() const { return loop_; }
		bool retry() const;
		void enableRetry() { retry_ = true; }

		const std::string& name() const
		{
			return name_;
		}

		/// Set connection callback.
		/// Not thread safe.
		void setConnectionCallBack(const ConnectionCallBack& cb)
		{
			connectionCallBack_ = cb;
		}

		/// Set message callback.
		/// Not thread safe.
		void setMessageCallBack(const MessageCallBack& cb)
		{
			messageCallBack_ = cb;
		}

		/// Set write complete callback.
		/// Not thread safe.
		void setWriteCompleteCallBack(const WriteCompleteCallBack& cb)
		{
			writeCompleteCallBack_ = cb;
		}
	private:

		/// Not thread safe, but in loop
		void newConnection(int sockfd);
		/// Not thread safe, but in loop
		void removeConnection(const TcpConnectionPtr& conn);
		EventLoop* loop_;
		ConnectorPtr connector_; // avoid revealing Connector
		const std::string name_;
		ConnectionCallBack connectionCallBack_;
		MessageCallBack messageCallBack_;
		WriteCompleteCallBack writeCompleteCallBack_;
		bool retry_;   // atomic
		bool connect_; // atomic
		// always in loop thread
		int nextConnId_;
		mutable std::mutex mutex_;
		TcpConnectionPtr connection_; // @GuardedBy mutex_
	};

}