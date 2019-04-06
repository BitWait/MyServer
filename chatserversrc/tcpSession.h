#ifndef __TCPSESSION_H__
#define __TCPSESSION_H__

#include <memory>
#include "../net/tcpconnection.h"

using namespace net;

class TcpSession
{
public:
	TcpSession(const std::weak_ptr<TcpConnection>& tmpconn);
	~TcpSession();
	TcpSession(const TcpSession& rhs) = delete;
	TcpSession& operator =(const TcpSession& rhs) = delete;

	std::shared_ptr<TcpConnection> GetConnectionPtr()
	{
		if (tmpConn_.expired())
			return NULL;

		return tmpConn_.lock();
	}

	void Send(int32_t cmd, int32_t seq, const std::string& data);
	void Send(int32_t cmd, int32_t seq, const char* data, int32_t dataLength);
	void Send(const std::string& p);
	void Send(const char* p, int32_t length);

private:
	void SendPackage(const char* p, int32_t length);
protected:
	//TcpSession����TcpConnection���������ָ�룬��ΪTcpConnection���ܻ�����������Լ����٣���ʱTcpSessionӦ��ҲҪ����
	std::weak_ptr<TcpConnection>    tmpConn_;
};



#endif
