#include "chatSession.h"
#include <string.h>
#include <sstream>
#include <list>
#include "../net/tcpconnection.h"
#include "../net/protocolstream.h"
#include "../base/logging.h"
#include "../base/singleton.h"
#include "../jsoncpp-0.5.0/json.h"
#include "msg.h"
//#include "UserManager.h"
#include "IMServer.h"
//#include "MsgCacheManager.h"
#include "../zlib1.2.11/ZlibUtil.h"
#include "bussinessLogic.h"

//������ֽ�������Ϊ10M
#define MAX_PACKAGE_SIZE    10 * 1024 * 1024

using namespace std;
using namespace net;

//��������ʱ���ݰ�����������������ó�30��
#define MAX_NO_PACKAGE_INTERVAL  30

ChatSession::ChatSession(const std::shared_ptr<TcpConnection>& conn, int sessionid) :
TcpSession(conn),
m_id(sessionid),
m_seq(0),
m_isLogin(false)
{
	m_userinfo.userid = 0;
	m_lastPackageTime = time(NULL);

	//����ע�͵��������ڵ���
	//EnableHearbeatCheck();
}

ChatSession::~ChatSession()
{

}

void ChatSession::OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime)
{
	while (true)
	{
		//����һ����ͷ��С
		if (pBuffer->readableBytes() < (size_t)sizeof(msg))
		{
			//LOG_INFO << "buffer is not enough for a package header, pBuffer->readableBytes()=" << pBuffer->readableBytes() << ", sizeof(msg)=" << sizeof(msg);
			return;
		}

		//ȡ��ͷ��Ϣ
		msg header;
		memcpy(&header, pBuffer->peek(), sizeof(msg));
		//���ݰ�ѹ����
		if (header.compressflag == PACKAGE_COMPRESSED)
		{
			//��ͷ�д��������ر�����
			if (header.compresssize <= 0 || header.compresssize > MAX_PACKAGE_SIZE ||
				header.originsize <= 0 || header.originsize > MAX_PACKAGE_SIZE)
			{
				//�ͻ��˷��Ƿ����ݰ��������������ر�֮
				LOG_ERROR << "Illegal package, compresssize: " << header.compresssize
					<< ", originsize: " << header.originsize
					<< ", close TcpConnection, client : " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			//�յ������ݲ���һ�������İ�
			if (pBuffer->readableBytes() < (size_t)header.compresssize + sizeof(msg))
				return;

			pBuffer->retrieve(sizeof(msg));
			std::string inbuf;
			inbuf.append(pBuffer->peek(), header.compresssize);
			pBuffer->retrieve(header.compresssize);
			std::string destbuf;
			if (!ZlibUtil::UncompressBuf(inbuf, destbuf, header.originsize))
			{
				LOG_ERROR << "uncompress error, client: " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			if (!Process(conn, destbuf.c_str(), destbuf.length()))
			{
				//�ͻ��˷��Ƿ����ݰ��������������ر�֮
				LOG_ERROR << "Process error, close TcpConnection, client: " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			m_lastPackageTime = time(NULL);
		}
		//���ݰ�δѹ��
		else
		{
			//��ͷ�д��������ر�����
			if (header.originsize <= 0 || header.originsize > MAX_PACKAGE_SIZE)
			{
				//�ͻ��˷��Ƿ����ݰ��������������ر�֮
				LOG_ERROR << "Illegal package, originsize: " << header.originsize
					<< ", close TcpConnection, client : " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			//�յ������ݲ���һ�������İ�
			if (pBuffer->readableBytes() < (size_t)header.originsize + sizeof(msg))
				return;

			pBuffer->retrieve(sizeof(msg));
			std::string inbuf;
			inbuf.append(pBuffer->peek(), header.originsize);
			pBuffer->retrieve(header.originsize);
			if (!Process(conn, inbuf.c_str(), inbuf.length()))
			{
				//�ͻ��˷��Ƿ����ݰ��������������ر�֮
				LOG_ERROR << "Process error, close TcpConnection, client: " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			m_lastPackageTime = time(NULL);
		}// end else

	}// end while-loop

}

bool ChatSession::Process(const std::shared_ptr<TcpConnection>& conn, const char* inbuf, size_t buflength)
{
	BinaryReadStream readStream(inbuf, buflength);
	int32_t cmd;
	if (!readStream.ReadInt32(cmd))
	{
		LOG_WARN << "read cmd error, client: " << conn->peerAddress().toIpPort();
		return false;
	}

	//int seq;
	if (!readStream.ReadInt32(m_seq))
	{
		LOG_ERROR << "read seq error, client: " << conn->peerAddress().toIpPort();
		return false;
	}

	std::string data;
	size_t datalength;
	if (!readStream.ReadString(&data, 0, datalength))
	{
		LOG_ERROR << "read data error, client: " << conn->peerAddress().toIpPort();
		return false;
	}

	//������̫Ƶ��������ӡ
	if (cmd != msg_type_heartbeat)
		LOG_INFO << "Request from client: userid=" << m_userinfo.userid << ", cmd=" << cmd << ", seq=" << m_seq << ", data=" << data << ", datalength=" << datalength << ", buflength=" << buflength;
	//LOG_DEBUG_BIN((unsigned char*)inbuf, length);

	switch (cmd)
	{
		//������
	case msg_type_heartbeat:
		OnHeartbeatResponse(conn);
		break;
	}

	m_seq++;
	return true;

}

void ChatSession::OnHeartbeatResponse(const std::shared_ptr<TcpConnection>& conn)
{
	std::string dummydata;
	Send(msg_type_heartbeat, m_seq, dummydata);

	//��������־�Ͳ�Ҫ��ӡ�ˣ�������д����־
	//LOG_INFO << "Response to client: cmd=1000" << ", sessionId=" << m_id;
}