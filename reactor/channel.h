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

		int index() { return index_; }
		void set_index(int idx) { index_ = idx; }

		/* ���س��б�Channel��EventLoop ָ�� */
		EventLoop* ownerLoop() { return loop_; }
	private:
		void update();

		static const int            kNoneEvent;
		static const int            kReadEvent;
		static const int            kWriteEvent;

		EventLoop*                  loop_;		//��Channel������EventLoop
		const int                   fd_;		//��Channel������ļ���������Channel��ӵ��fd
		int                         events_;	//���ĵ�IO�¼������û�����
		int                         revents_;	//Ŀǰ����¼�
		int                         index_;		//��Pollerʹ�õ��±� used by Poller.

		EventCallBack               writeCallBack_;
		EventCallBack               closeCallBack_;
		EventCallBack               errorCallBack_;
		EventCallBack               readCallBack_;
	};

}