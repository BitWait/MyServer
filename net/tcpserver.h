#pragma once

#include <atomic>
//#include <cstdatomic> // 老gcc头文件
#include <map>
#include <memory>

#include "tcpconnection.h"

namespace net
{

	class Accept;
	class EventLoop;
	class EventLoopThreadPool;

	///
	/// TCP server, supports single-threaded and thread-pool models.
	///
	/// This is an interface class, so don't expose too much details.
	class TcpServer
	{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallBack;
		enum Option
		{
			kNoReusePort,
			kReusePort,
		};

		//TcpServer(EventLoop* loop, const InetAddress& listenAddr);
		TcpServer(EventLoop* loop,
			const InetAddress& listenAddr,
			const std::string& nameArg,
			Option option = kReusePort);      //TODO: 默认修改成kReusePort
		~TcpServer();  // force out-line dtor, for scoped_ptr members.

		const std::string& hostport() const { return hostport_; }
		const std::string& name() const { return name_; }
		EventLoop* getLoop() const { return loop_; }

		/// Set the number of threads for handling input.
		///
		/// Always accepts new connection in loop's thread.
		/// Must be called before @c start
		/// @param numThreads
		/// - 0 means all I/O in loop's thread, no thread will created.
		///   this is the default value.
		/// - 1 means all I/O in another thread.
		/// - N means a thread pool with N threads, new connections
		///   are assigned on a round-robin basis.

		//void setThreadNum(int numThreads);
		void setThreadInitCallBack(const ThreadInitCallBack& cb)
		{
			threadInitCallBack_ = cb;
		}
		/// valid after calling start()
		//std::shared_ptr<EventLoopThreadPool> threadPool()
		//{ return threadPool_; }

		/// Starts the server if it's not listenning.
		///
		/// It's harmless to call it multiple times.
		/// Thread safe.
		void start();

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

		void removeConnection(const TcpConnectionPtr& conn);

	private:
		/// Not thread safe, but in loop
		void newConnection(int sockfd, const InetAddress& peerAddr);
		/// Thread safe.

		/// Not thread safe, but in loop
		void removeConnectionInLoop(const TcpConnectionPtr& conn);

		typedef std::map<string, TcpConnectionPtr> ConnectionMap;

	private:
		EventLoop*                  loop_;  // the acceptor loop
		const string                hostport_;
		const string                name_;
		std::shared_ptr<Accept>   acceptor_; // avoid revealing Acceptor
		//std::shared_ptr<EventLoopThreadPool> threadPool_;
		ConnectionCallBack          connectionCallBack_;
		MessageCallBack             messageCallBack_;
		WriteCompleteCallBack       writeCompleteCallBack_;
		ThreadInitCallBack          threadInitCallBack_;
		std::atomic<int>            started_;
		int                         nextConnId_;  // always in loop thread
		ConnectionMap               connections_;
	};

}
