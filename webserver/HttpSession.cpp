#include "../net/http/HttpContext.h"
#include "../net/http/HttpResponse.h"
#include "HttpSession.h"
#include <string>

using namespace net;

void HttpSession::OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* pBuffer, Timestamp receivTime)
{
	HttpContext context;

	if (!context.parseRequest(pBuffer, receivTime))
	{
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
	}

	if (context.gotAll())
	{
		Process(conn, context.request());
		context.reset();
	}
}

void HttpSession::Process(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& req)
{
	const std::string& connection = req.getHeader("Connection");
	bool close = connection == "close" ||
		(req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
	net::HttpResponse response(close);
	msgCallBack_(req, &response);
	net::Buffer buf;
	response.appendToBuffer(&buf);
	conn->send(&buf);
	if (response.closeConnection())
	{
		conn->shutdown();
	}
}



