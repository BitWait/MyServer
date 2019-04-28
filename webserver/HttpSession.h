/**
* Httpª·ª∞¿‡, HttpSession.h
*/
#ifndef __HTTP_SESSION_H__
#define __HTTP_SESSION_H__

#include "../net/buffer.h"
#include "../base/timestamp.h"
#include "../net/tcpconnection.h"
#include "../net/http/HttpRequest.h"
#include "../net/http/HttpResponse.h"
#include <memory>
#include <functional>

class net::HttpRequest;
class net::HttpResponse;
class HttpSession
{
public:
	typedef std::function<void(const net::HttpRequest& req, net::HttpResponse* resp)> MsgCallBack;
	HttpSession() = default;
	~HttpSession() = default;
	HttpSession(const HttpSession& rhs) = delete;
	HttpSession& operator =(const HttpSession& rhs) = delete;
	void setMsgCallBack(const MsgCallBack& cb)
	{
		msgCallBack_ = cb;
	}

public:
	void OnRead(const std::shared_ptr<net::TcpConnection>& conn, net::Buffer* pBuffer, Timestamp receivTime);

private:
	void Process(const std::shared_ptr<net::TcpConnection>& conn, const net::HttpRequest& req);

private:
	MsgCallBack msgCallBack_;
};


#endif 