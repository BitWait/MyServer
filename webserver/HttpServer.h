/**
* 聊天服务支持http请求, HttpServer.h
* zhangyl 2018.05.16
*/
#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <memory>
#include <mutex>
#include <list>
#include "../net/eventloop.h"
#include "../net/tcpserver.h"
#include "../net/http/HttpRequest.h"
#include "../net/http/HttpResponse.h"
#include "HttpSession.h"

using namespace net;

class HttpSession;

class HttpServer final
{
public:
	HttpServer() = default;
	~HttpServer() = default;

	HttpServer(const HttpServer& rhs) = delete;
	HttpServer& operator =(const HttpServer& rhs) = delete;

public:
	bool Init(const char* ip, short port, EventLoop* loop);

	//新连接到来调用或连接断开，所以需要通过conn->connected()来判断，一般只在主loop里面调用
	void OnConnection(std::shared_ptr<TcpConnection> conn);
	//连接断开
	void OnClose(const std::shared_ptr<TcpConnection>& conn);
	void onRequest(const HttpRequest& req, HttpResponse* resp);

private:
	std::shared_ptr<TcpServer>                     m_server;
	HttpSession m_session;
};


#endif //!__HTTP_SERVER_H__