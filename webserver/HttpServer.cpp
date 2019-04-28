
#include "../net/inetaddress.h"
#include "../base/logging.h"
#include "../base/singleton.h"
#include "HttpSession.h"
#include "HttpServer.h"

bool HttpServer::Init(const char* ip, short port, EventLoop* loop)
{
	InetAddress addr(ip, port);
	m_server.reset(new TcpServer(loop, addr, "HTTPSERVER", TcpServer::kReusePort));
	m_session.setMsgCallBack(std::bind(&HttpServer::onRequest, this, std::placeholders::_1, std::placeholders::_2));
	m_server->setConnectionCallBack(std::bind(&HttpServer::OnConnection, this, std::placeholders::_1));

	m_server->setMessageCallBack(std::bind(&HttpSession::OnRead, &m_session, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//Æô¶¯ÕìÌý
	m_server->start();
	return true;
}

void HttpServer::OnConnection(std::shared_ptr<TcpConnection> conn)
{
	LOG_INFO << conn->localAddress().toIpPort() << " -> "
		<< conn->peerAddress().toIpPort() << " is "
		<< (conn->connected() ? "UP" : "DOWN");

	if (conn->connected())
	{

	}
	else
	{

	}
}


void HttpServer::onRequest(const HttpRequest& req, HttpResponse* resp)
{
	std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;

	const std::map<string, string>& headers = req.headers();
	for (const auto& header : headers)
	{
		std::cout << header.first << ": " << header.second << std::endl;
	}

	if (req.path() == "/")
	{
		resp->setStatusCode(HttpResponse::k200Ok);
		resp->setStatusMessage("OK");
		resp->setContentType("text/html");
		resp->addHeader("Server", "BitWait");
		string now = Timestamp::now().toFormattedString();
		resp->setBody("<html><head><title>This is title</title></head>"
			"<body><h1>Hello</h1>Now is " + now +
			"</body></html>");
	}
	else if (req.path() == "/hello")
	{
		resp->setStatusCode(HttpResponse::k200Ok);
		resp->setStatusMessage("OK");
		resp->setContentType("text/plain");
		resp->addHeader("Server", "BitWait");
		resp->setBody("hello, world!\n");
	}
	else
	{
		resp->setStatusCode(HttpResponse::k404NotFound);
		resp->setStatusMessage("Not Found");
		resp->setCloseConnection(true);
	}
}