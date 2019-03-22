#pragma once
/* 事件分发类，主要包括 fd fd监听的事件、事件回调函数 */
#include <memory>
#include <functional>

#include "../base/timestamp.h"
namespace net{

	class EventLoop;

	class Channel
	{
	public:
		/* 事件回调函数模板 */
		typedef std::function<void()> EventCallBack;
		typedef std::function<void(Timestamp)> ReadEventCallBack;/* 读操作回调函数，需要传入时间 */
		
		/*
		* 一个Channel只负责一个fd，但Channel不拥有fd
		* EventLoop调用Poller监听事件集合，就绪的事件元素就是Channel
		* Channel不仅是返回就绪事件，还可以处理事件
		*/
		Channel(EventLoop* loop, int fd);
		//~Channel();

		/*
		* Channel的核心
		* 处理事件，一般由Poller通过EventLoop来调用
		* 当fd对应的事件就绪后Channel::handleEvent()执行相应的事件回调
		* 如可读事件执行 readCallback_()
		*/
		void handleEvent(Timestamp recvTime);

		void setReadCallBack(const ReadEventCallBack& cb)
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

		void tie(const std::shared_ptr<void>&);

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
		/* 销毁读事件 */
		void disableReading() { events_ &= ~kReadEvent; update(); }
		/* fd 注册可写事件 */
		void enableWriting() { events_ |= kWriteEvent; update(); }
		/* 销毁写事件 */
		void disableWriting() { events_ &= ~kWriteEvent; update(); }
		/* 停止监听所有事件 */
		void disableAll() { events_ = kNoneEvent; update(); }
		/* 是否注册了读写事件 */
		bool isWriting() const { return events_ & kWriteEvent; }

		// for Poller
		int index() { return index_; }
		void set_index(int idx) { index_ = idx; }

		// for debug
		string reventsToString() const;

		void doNotLogHup() { logHup_ = false; }
		/* 返回持有本Channel的EventLoop 指针 */
		EventLoop* ownerLoop() { return loop_; }
		/* 将Channel 从EventLoop中移除 */
		void remove();

	private:
		/* 通过调用loop_->updateChannel()来注册或改变本fd在epoll中监听的事件 */
		void update();
		void handleEventWithGuard(Timestamp recvTime);

		static const int            kNoneEvent;
		static const int            kReadEvent;
		static const int            kWriteEvent;
		
		EventLoop*                  loop_;		//本Channel所属的EventLoop
		const int                   fd_;		//本Channel负责的文件描述符，Channel不拥有fd
		int                         events_;	//关心的IO事件，由用户设置
		int                         revents_;	//目前活动的事件
		int                         index_;		//被Poller使用的下标 used by Poller.
		bool                        logHup_;	//是否生成某些日志

		std::weak_ptr<void>         tie_;
		bool                        tied_;
		bool                        eventHandling_;	 //是否正在处理事件
		bool                        addedToLoop_;

		ReadEventCallBack           readCallBack_;
		EventCallBack               writeCallBack_;
		EventCallBack               closeCallBack_;
		EventCallBack               errorCallBack_;

	};
}
