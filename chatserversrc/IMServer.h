#pragma once
#include <memory>
#include <list>
#include <map>
#include <mutex>
#include "../net/tcpserver.h"
#include "../net/eventloop.h"
#include "chatSession.h"

using namespace net;

enum CLIENT_TYPE
{
	CLIENT_TYPE_UNKOWN,
	CLIENT_TYPE_PC,
	CLIENT_TYPE_ANDROID,
	CLIENT_TYPE_IOS,
	CLIENT_TYPE_MAC
};

struct StoredUserInfo
{
	int32_t         userid;
	std::string     username;
	std::string     password;
	std::string     nickname;
};

class IMServer final
{
public:
	IMServer() = default;
	~IMServer() = default;

	IMServer(const IMServer& rhs) = delete;
	IMServer& operator =(const IMServer& rhs) = delete;

	bool Init(const char* ip, short port, EventLoop* loop);

	void GetSessions(std::list<std::shared_ptr<ChatSession>>& sessions);

private:
	//�����ӵ������û����ӶϿ���������Ҫͨ��conn->connected()���жϣ�һ��ֻ����loop�������
	void OnConnection(std::shared_ptr<TcpConnection> conn);
	//���ӶϿ�
	void OnClose(const std::shared_ptr<TcpConnection>& conn);
private:
	std::shared_ptr<TcpServer>                     m_server;
	std::list<std::shared_ptr<ChatSession>>      m_sessions;
	std::mutex                                     m_sessionMutex;      //���߳�֮�䱣��m_sessions
	int                                            m_sessionId{};
	std::mutex                                     m_idMutex;           //���߳�֮�䱣��m_baseUserId
};
