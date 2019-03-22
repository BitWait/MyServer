#pragma once
/* �¼��ַ��࣬��Ҫ���� fd fd�������¼����¼��ص����� */
#include <memory>
#include <functional>

#include "../base/timestamp.h"
namespace net{

	class EventLoop;

	class Channel
	{
	public:
		/* �¼��ص�����ģ�� */
		typedef std::function<void()> EventCallBack;
		typedef std::function<void(Timestamp)> ReadEventCallBack;/* �������ص���������Ҫ����ʱ�� */
		
		/*
		* һ��Channelֻ����һ��fd����Channel��ӵ��fd
		* EventLoop����Poller�����¼����ϣ��������¼�Ԫ�ؾ���Channel
		* Channel�����Ƿ��ؾ����¼��������Դ����¼�
		*/
		Channel(EventLoop* loop, int fd);
		//~Channel();

		/*
		* Channel�ĺ���
		* �����¼���һ����Pollerͨ��EventLoop������
		* ��fd��Ӧ���¼�������Channel::handleEvent()ִ����Ӧ���¼��ص�
		* ��ɶ��¼�ִ�� readCallback_()
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

		int fd() const { return fd_; }/* ���ظ�Channel�����fd*/
		int events() const { return events_; }/* ���� fd  ע����¼� */

		/*
		* ����poll����epoll_wait�󣬸���fd�ķ����¼����ô˺���,�趨fd�ľ����¼�����
		* handleEvent ���ݾ����¼�����(revents_)������ִ���ĸ��¼��ص�����
		*/
		void set_revents(int revt) { revents_ = revt; } // used by pollers

		bool isNoneEvent() const { return events_ == kNoneEvent; }/* �ж�fd�ǲ��� û�� �¼����� */
		/* fd ע��ɶ��¼� */
		void enableReading() { events_ |= kReadEvent; update(); }/* update ͨ��eventloop ȥ����epoll��fd�ļ����¼� */
		/* ���ٶ��¼� */
		void disableReading() { events_ &= ~kReadEvent; update(); }
		/* fd ע���д�¼� */
		void enableWriting() { events_ |= kWriteEvent; update(); }
		/* ����д�¼� */
		void disableWriting() { events_ &= ~kWriteEvent; update(); }
		/* ֹͣ���������¼� */
		void disableAll() { events_ = kNoneEvent; update(); }
		/* �Ƿ�ע���˶�д�¼� */
		bool isWriting() const { return events_ & kWriteEvent; }

		// for Poller
		int index() { return index_; }
		void set_index(int idx) { index_ = idx; }

		// for debug
		string reventsToString() const;

		void doNotLogHup() { logHup_ = false; }
		/* ���س��б�Channel��EventLoop ָ�� */
		EventLoop* ownerLoop() { return loop_; }
		/* ��Channel ��EventLoop���Ƴ� */
		void remove();

	private:
		/* ͨ������loop_->updateChannel()��ע���ı䱾fd��epoll�м������¼� */
		void update();
		void handleEventWithGuard(Timestamp recvTime);

		static const int            kNoneEvent;
		static const int            kReadEvent;
		static const int            kWriteEvent;
		
		EventLoop*                  loop_;		//��Channel������EventLoop
		const int                   fd_;		//��Channel������ļ���������Channel��ӵ��fd
		int                         events_;	//���ĵ�IO�¼������û�����
		int                         revents_;	//Ŀǰ����¼�
		int                         index_;		//��Pollerʹ�õ��±� used by Poller.
		bool                        logHup_;	//�Ƿ�����ĳЩ��־

		std::weak_ptr<void>         tie_;
		bool                        tied_;
		bool                        eventHandling_;	 //�Ƿ����ڴ����¼�
		bool                        addedToLoop_;

		ReadEventCallBack           readCallBack_;
		EventCallBack               writeCallBack_;
		EventCallBack               closeCallBack_;
		EventCallBack               errorCallBack_;

	};
}
