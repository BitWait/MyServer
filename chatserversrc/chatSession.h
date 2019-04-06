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
	int32_t     clienttype;     //�ͻ�������, 0δ֪, pc=1, android/ios=2
	int32_t     status;         //����״̬ 0���� 1���� 2æµ 3�뿪 4����
};

class ChatSession:public TcpSession
{
public:
	ChatSession(const std::shared_ptr<TcpConnection>& conn, int sessionid);
	virtual ~ChatSession();

	ChatSession(const ChatSession& rhs) = delete;
	ChatSession& operator =(const ChatSession& rhs) = delete;

	//�����ݿɶ�, �ᱻ�������loop����
	void OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime);

	//��������������ָ��ʱ���ڣ�������30�룩δ�յ����ݰ����������Ͽ��ڿͻ��˵�����
	void CheckHeartbeat(const std::shared_ptr<TcpConnection>& conn);
private:
	bool Process(const std::shared_ptr<TcpConnection>& conn, const char* inbuf, size_t buflength);

	void OnHeartbeatResponse(const std::shared_ptr<TcpConnection>& conn);
private:
	int32_t           m_id;                 //session id
	OnlineUserInfo    m_userinfo;
	int32_t           m_seq;                //��ǰSession���ݰ����к�
	bool              m_isLogin;            //��ǰSession��Ӧ���û��Ƿ��Ѿ���¼
	time_t            m_lastPackageTime;    //��һ���շ�����ʱ��
	TimerId           m_checkOnlineTimerId; //����Ƿ����ߵĶ�ʱ��id
};

