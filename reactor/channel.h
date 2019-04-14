#pragma once

#include <functional>
namespace reactor{

	class EventLoop;
	class Channel
	{
	public:
		typedef std::function<void()> EventCallBack;
		
		Channel(EventLoop* loop, int fd);

		void handleEvent();

		void setReadCallBack(const EventCallBack& cb)
		{
			readCallBack_ = cb;
		}
		void setWriteCallBack(const EventCallBack& cb)
		{
			writeCallBack_ = cb;
		}
		void setCloseCallBack(const EventCallBack& cb)
		{
			closeCallBack_ = cb;
		}
		void setErrorCallBack(const EventCallBack& cb)
		{
			errorCallBack_ = cb;
		}

		int fd() const { return fd_; }/* 返回该Channel负责的fd*/
		int events() const { return events_; }/* 返回 fd  注册的事件 */

		/*
		* 进行poll或者epoll_wait后，根据fd的返回事件调用此函数,设定fd的就绪事件类型
		* handleEvent 根据就绪事件类型(revents_)来决定执行哪个事件回调函数
		*/
		void set_revents(int revt) { revents_ = revt; } // used by pollers

		bool isNoneEvent() const { return events_ == kNoneEvent; }/* 判断fd是不是 没有 事件监听 */

		/* fd 注册可读事件 */
		void enableReading() { events_ |= kReadEvent; update(); }/* update 通过eventloop 去更新epoll中fd的监听事件 */

		int index() { return index_; }
		void set_index(int idx) { index_ = idx; }

		/* 返回持有本Channel的EventLoop 指针 */
		EventLoop* ownerLoop() { return loop_; }
	private:
		void update();

		static const int            kNoneEvent;
		static const int            kReadEvent;
		static const int            kWriteEvent;

		EventLoop*                  loop_;		//本Channel所属的EventLoop
		const int                   fd_;		//本Channel负责的文件描述符，Channel不拥有fd
		int                         events_;	//关心的IO事件，由用户设置
		int                         revents_;	//目前活动的事件
		int                         index_;		//被Poller使用的下标 used by Poller.

		EventCallBack               writeCallBack_;
		EventCallBack               closeCallBack_;
		EventCallBack               errorCallBack_;
		EventCallBack               readCallBack_;
	};

}