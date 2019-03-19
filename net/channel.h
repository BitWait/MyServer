#pragma once

#include <memory>
#include <functional>

#include "../base/timestamp.h"
namespace net{

	class EventLoop;

	class Channel
	{
	public:
		typedef std::function<void()> EventCallBack;
		typedef std::function<void(Timestamp)> ReadEventCallBack;

		Channel(EventLoop* loop, int fd);
		//~Channel();
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

		int fd() const { return fd_; }
		int events() const { return events_; }
		void set_revents(int revt) { revents_ = revt; } // used by pollers

		bool isNoneEvent() const { return events_ == kNoneEvent; }

		void enableReading() { events_ |= kReadEvent; update(); }
		void disableReading() { events_ &= ~kReadEvent; update(); }
		void enableWriting() { events_ |= kWriteEvent; update(); }
		void disableWriting() { events_ &= ~kWriteEvent; update(); }
		void disableAll() { events_ = kNoneEvent; update(); }
		bool isWriting() const { return events_ & kWriteEvent; }

		// for Poller
		int index() { return index_; }
		void set_index(int idx) { index_ = idx; }

		// for debug
		string reventsToString() const;

		void doNotLogHup() { logHup_ = false; }

		EventLoop* ownerLoop() { return loop_; }
		void remove();

	private:
		void update();
		void handleEventWithGuard(Timestamp recvTime);

		static const int            kNoneEvent;
		static const int            kReadEvent;
		static const int            kWriteEvent;
		
		EventLoop*                  loop_;
		const int                   fd_;
		int                         events_;
		int                         revents_;
		int                         index_;
		bool                        logHup_;

		std::weak_ptr<void>         tie_;
		bool                        tied_;
		bool                        eventHandling_;
		bool                        addedToLoop_;

		ReadEventCallBack           readCallBack_;
		EventCallBack               writeCallBack_;
		EventCallBack               closeCallBack_;
		EventCallBack               errorCallBack_;

	};
}
