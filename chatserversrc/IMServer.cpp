#include "../net/inetaddress.h"
#include "../base/logging.h"
#include "../base/singleton.h"
#include "IMServer.h"
#include "chatSession.h"
//#include "UserManager.h"

bool IMServer::Init(const char* ip, short port, EventLoop* loop)
{
	InetAddress addr(ip, port);
	m_server.reset(new TcpServer(loop, addr, "FLAMINGO-SERVER", TcpServer::kReusePort));
	m_server->setConnectionCallBack(std::bind(&IMServer::OnConnection, this, std::placeholders::_1));
	//Æô¶¯ÕìÌý
	m_server->start();

	return true;
}

void IMServer::OnConnection(std::shared_ptr<TcpConnection> conn)
{
	if (conn->connected())
	{
		//LOG_INFO << "client connected:" << conn->peerAddress().toIpPort();
		++m_sessionId;
		std::shared_ptr<ChatSession> spSession(new ChatSession(conn, m_sessionId));
		conn->setMessageCallBack(std::bind(&ChatSession::OnRead, spSession.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		std::lock_guard<std::mutex> guard(m_sessionMutex);
		m_sessions.push_back(spSession);
	}
	else
	{
		//OnClose(conn);
	}
}

