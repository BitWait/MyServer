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
	//新连接到来调用或连接断开，所以需要通过conn->connected()来判断，一般只在主loop里面调用
	void OnConnection(std::shared_ptr<TcpConnection> conn);
	//连接断开
	void OnClose(const std::shared_ptr<TcpConnection>& conn);
private:
	std::shared_ptr<TcpServer>                     m_server;
	std::list<std::shared_ptr<ChatSession>>      m_sessions;
	std::mutex                                     m_sessionMutex;      //多线程之间保护m_sessions
	int                                            m_sessionId{};
	std::mutex                                     m_idMutex;           //多线程之间保护m_baseUserId
};
