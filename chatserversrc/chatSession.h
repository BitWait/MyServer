#pragma once
#include "../net/buffer.h"
#include "../net/timerId.h"
#include "tcpSession.h"
using namespace net;

struct OnlineUserInfo
{
	int32_t     userid;
	std::string username;
	std::string nickname;
	std::string password;
	int32_t     clienttype;     //客户端类型, 0未知, pc=1, android/ios=2
	int32_t     status;         //在线状态 0离线 1在线 2忙碌 3离开 4隐身
};

class ChatSession:public TcpSession
{
public:
	ChatSession(const std::shared_ptr<TcpConnection>& conn, int sessionid);
	virtual ~ChatSession();

	ChatSession(const ChatSession& rhs) = delete;
	ChatSession& operator =(const ChatSession& rhs) = delete;

	//有数据可读, 会被多个工作loop调用
	void OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime);

	int32_t GetSessionId()
	{
		return m_id;
	}

	int32_t GetUserId()
	{
		return m_userinfo.userid;
	}

	std::string GetUsername()
	{
		return m_userinfo.username;
	}

	std::string GetNickname()
	{
		return m_userinfo.nickname;
	}

	std::string GetPassword()
	{
		return m_userinfo.password;
	}

	int32_t GetClientType()
	{
		return m_userinfo.clienttype;
	}

	int32_t GetUserStatus()
	{
		return m_userinfo.status;
	}

	int32_t GetUserClientType()
	{
		return m_userinfo.clienttype;
	}

	//检测心跳包，如果指定时间内（现在是30秒）未收到数据包，则主动断开于客户端的连接
	void CheckHeartbeat(const std::shared_ptr<TcpConnection>& conn);
private:
	bool Process(const std::shared_ptr<TcpConnection>& conn, const char* inbuf, size_t buflength);

	void OnHeartbeatResponse(const std::shared_ptr<TcpConnection>& conn);
private:
	int32_t           m_id;                 //session id
	OnlineUserInfo    m_userinfo;
	int32_t           m_seq;                //当前Session数据包序列号
	bool              m_isLogin;            //当前Session对应的用户是否已经登录
	time_t            m_lastPackageTime;    //上一次收发包的时间
	TimerId           m_checkOnlineTimerId; //检测是否在线的定时器id
};

