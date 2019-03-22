#pragma once

#include "callback.h"
#include "buffer.h"
#include "inetaddress.h"

#include <memory>

struct tcp_info;

namespace net{
	class Channel;
	class EventLoop;
	class Socket;

	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:
		TcpConnection(EventLoop* loop,
			const string& naem,
			int sockfd,
			const InetAddress& localAddr,
			const InetAddress& peerAddr);
		~TcpConnection();

		EventLoop* getLoop() const { return loop_; }
		const string& name() const { return name_; }
		const InetAddress& localAddress() const { return localAddr_; }
		const InetAddress& peerAddress() const { return peerAddr_; }
		bool connected() const { return state_ == kConnected; }

		bool getTcpInfo(struct tcp_info*) const;
		string getTcpInfoString() const;

		void send(const void* message, int len);
		void send(const string& message);
		// void send(Buffer&& message); // C++11
		void send(Buffer* message);  // this one will swap data
		void shutdown(); // NOT thread safe, no simultaneous calling
		// void shutdownAndForceCloseAfter(double seconds); // NOT thread safe, no simultaneous calling
		void forceClose();
		void setTcpNoDelay(bool on);

		void setConnectionCallBack(const ConnectionCallBack& cb)
		{
			connectionCallBack_ = cb;
		}

		void setMessageCallBack(const MessageCallBack& cb)
		{
			messageCallBack_ = cb;
		}

		//设置成功发完数据执行的回调
		void setWriteCompleteCallBack(const WriteCompleteCallBack& cb)
		{
			writeCompleteCallBack_ = cb;
		}

		void setHighWaterMarkCallBack(const HighWaterMarkCallBack& cb, size_t highWaterMark)
		{
			highWaterMarkCallBack_ = cb; 
			highWaterMark_ = highWaterMark;
		}

		/// Advanced interface
		Buffer* inputBuffer()
		{
			return &inputBuffer_;
		}

		Buffer* outputBuffer()
		{
			return &outputBuffer_;
		}

		/// Internal use only.
		void setCloseCallBack(const CloseCallBack& cb)
		{
			closeCallBack_ = cb;
		}

		// called when TcpServer accepts a new connection
		void connectEstablished();   // should be called only once
		// called when TcpServer has removed me from its map
		void connectDestroyed();  // should be called only once

	private:
		enum StateE 
		{ 
			kDisconnected, 
			kConnecting, 
			kConnected, 
			kDisconnecting 
		};

		void handleRead(Timestamp receiveTime);
		void handleWrite();
		void handleClose();
		void handleError();
		// void sendInLoop(string&& message);
		void sendInLoop(const string& message);
		void sendInLoop(const void* message, size_t len);
		void shutdownInLoop();
		// void shutdownAndForceCloseInLoop(double seconds);
		void forceCloseInLoop();
		void setState(StateE s) { state_ = s; }
		const char* stateToString() const;

	private:
		EventLoop*                  loop_;
		const string                name_;
		StateE                      state_;

		std::shared_ptr<Socket>     socket_;
		std::shared_ptr<Channel>    channel_;
		const InetAddress           localAddr_;
		const InetAddress           peerAddr_;

		ConnectionCallBack          connectionCallBack_;
		MessageCallBack             messageCallBack_;
		WriteCompleteCallBack       writeCompleteCallBack_;
		HighWaterMarkCallBack       highWaterMarkCallBack_;
		CloseCallBack               closeCallBack_;

		size_t                      highWaterMark_;
		Buffer                      inputBuffer_;
		Buffer                      outputBuffer_; // FIXME: use list<Buffer> as output buffer.
	};
}