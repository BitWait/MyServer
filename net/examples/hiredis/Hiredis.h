#ifndef MUDUO_CONTRIB_HIREDIS_HIREDIS_H
#define MUDUO_CONTRIB_HIREDIS_HIREDIS_H


#include <string>
#include "../../callback.h"
#include "../../inetaddress.h"
#include <hiredis/hiredis.h>

struct redisAsyncContext;


namespace net
{
	class Channel;
	class EventLoop;
}


namespace hiredis
{

	class Hiredis : public std::enable_shared_from_this<Hiredis>
	{
	public:
		typedef std::function<void(Hiredis*, int)> ConnectCallback;
		typedef std::function<void(Hiredis*, int)> DisconnectCallback;
		typedef std::function<void(Hiredis*, redisReply*)> CommandCallback;

		Hiredis(net::EventLoop* loop, const net::InetAddress& serverAddr);
		~Hiredis();

		const net::InetAddress& serverAddress() const { return serverAddr_; }
		// redisAsyncContext* context() { return context_; }
		bool connected() const;
		const char* errstr() const;

		void setConnectCallback(const ConnectCallback& cb) { connectCb_ = cb; }
		void setDisconnectCallback(const DisconnectCallback& cb) { disconnectCb_ = cb; }

		void connect();
		void disconnect();  // FIXME: implement this with redisAsyncDisconnect

		int command(const CommandCallback& cb, std::string cmd, ...);

		int ping();

	private:
		void handleRead(Timestamp receiveTime);
		void handleWrite();

		int fd() const;
		void logConnection(bool up) const;
		void setChannel();
		void removeChannel();

		void connectCallback(int status);
		void disconnectCallback(int status);
		void commandCallback(redisReply* reply, CommandCallback* privdata);

		static Hiredis* getHiredis(const redisAsyncContext* ac);

		static void connectCallback(const redisAsyncContext* ac, int status);
		static void disconnectCallback(const redisAsyncContext* ac, int status);
		// command callback
		static void commandCallback(redisAsyncContext* ac, void*, void*);

		static void addRead(void* privdata);
		static void delRead(void* privdata);
		static void addWrite(void* privdata);
		static void delWrite(void* privdata);
		static void cleanup(void* privdata);

		void pingCallback(Hiredis* me, redisReply* reply);

	private:
		net::EventLoop* loop_;
		const net::InetAddress serverAddr_;
		redisAsyncContext* context_;
		std::shared_ptr<net::Channel> channel_;
		ConnectCallback connectCb_;
		DisconnectCallback disconnectCb_;
	};

}  // namespace hiredis

#endif  // MUDUO_CONTRIB_HIREDIS_HIREDIS_H
