#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>

namespace net{
	class EventLoop;

	class EventLoopThread
	{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallBack;

		EventLoopThread(const ThreadInitCallBack& cb = ThreadInitCallBack(),
			const std::string& name = std::string());
		~EventLoopThread();
		EventLoop* startLoop();// 启动线程，该线程就成为了IO线程
	private:
		void threadFunc();

		EventLoop* loop_;
		bool exiting_;
		std::shared_ptr<std::thread> thread_;
		std::mutex mutex_;
		std::condition_variable cond_;
		ThreadInitCallBack callBack_;
	};
}


