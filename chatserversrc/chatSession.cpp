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

//包最大字节数限制为10M
#define MAX_PACKAGE_SIZE    10 * 1024 * 1024

using namespace std;
using namespace net;

//允许的最大时数据包来往间隔，这里设置成30秒
#define MAX_NO_PACKAGE_INTERVAL  30

ChatSession::ChatSession(const std::shared_ptr<TcpConnection>& conn, int sessionid) :
TcpSession(conn),
m_id(sessionid),
m_seq(0),
m_isLogin(false)
{
	m_userinfo.userid = 0;
	m_lastPackageTime = time(NULL);

	//暂且注释掉，不利于调试
	//EnableHearbeatCheck();
}

ChatSession::~ChatSession()
{

}

void ChatSession::OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime)
{
	while (true)
	{
		//不够一个包头大小
		if (pBuffer->readableBytes() < (size_t)sizeof(msg))
		{
			//LOG_INFO << "buffer is not enough for a package header, pBuffer->readableBytes()=" << pBuffer->readableBytes() << ", sizeof(msg)=" << sizeof(msg);
			return;
		}

		//取包头信息
		msg header;
		memcpy(&header, pBuffer->peek(), sizeof(msg));
		//数据包压缩过
		if (header.compressflag == PACKAGE_COMPRESSED)
		{
			//包头有错误，立即关闭连接
			if (header.compresssize <= 0 || header.compresssize > MAX_PACKAGE_SIZE ||
				header.originsize <= 0 || header.originsize > MAX_PACKAGE_SIZE)
			{
				//客户端发非法数据包，服务器主动关闭之
				LOG_ERROR << "Illegal package, compresssize: " << header.compresssize
					<< ", originsize: " << header.originsize
					<< ", close TcpConnection, client : " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			//收到的数据不够一个完整的包
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
				//客户端发非法数据包，服务器主动关闭之
				LOG_ERROR << "Process error, close TcpConnection, client: " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			m_lastPackageTime = time(NULL);
		}
		//数据包未压缩
		else
		{
			//包头有错误，立即关闭连接
			if (header.originsize <= 0 || header.originsize > MAX_PACKAGE_SIZE)
			{
				//客户端发非法数据包，服务器主动关闭之
				LOG_ERROR << "Illegal package, originsize: " << header.originsize
					<< ", close TcpConnection, client : " << conn->peerAddress().toIpPort();
				conn->forceClose();
				return;
			}

			//收到的数据不够一个完整的包
			if (pBuffer->readableBytes() < (size_t)header.originsize + sizeof(msg))
				return;

			pBuffer->retrieve(sizeof(msg));
			std::string inbuf;
			inbuf.append(pBuffer->peek(), header.originsize);
			pBuffer->retrieve(header.originsize);
			if (!Process(conn, inbuf.c_str(), inbuf.length()))
			{
				//客户端发非法数据包，服务器主动关闭之
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

	//心跳包太频繁，不打印
	if (cmd != msg_type_heartbeat)
		LOG_INFO << "Request from client: userid=" << m_userinfo.userid << ", cmd=" << cmd << ", seq=" << m_seq << ", data=" << data << ", datalength=" << datalength << ", buflength=" << buflength;
	//LOG_DEBUG_BIN((unsigned char*)inbuf, length);

	switch (cmd)
	{
		//心跳包
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

	//心跳包日志就不要打印了，很容易写满日志
	//LOG_INFO << "Response to client: cmd=1000" << ", sessionId=" << m_id;
}