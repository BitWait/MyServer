// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//

#include "HttpServer.h"
#include "../../base/logging.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace net;


namespace net
{
	namespace detail
	{

		void defaultHttpCallBack(const HttpRequest&, HttpResponse* resp)
		{
			resp->setStatusCode(HttpResponse::k404NotFound);
			resp->setStatusMessage("Not Found");
			resp->setCloseConnection(true);
		}

	}  // namespace detail
}  // namespace net


HttpServer::HttpServer(EventLoop* loop,
	const InetAddress& listenAddr,
	const string& name,
	TcpServer::Option option)
	: server_(loop, listenAddr, name, option),
	httpCallBack_(detail::defaultHttpCallBack)
{
	server_.setConnectionCallBack(
		std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
	server_.setMessageCallBack(
		std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::start()
{
	LOG_WARN << "HttpServer[" << server_.name()
		<< "] starts listenning on " << server_.hostport();
	server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
	if (conn->connected())
	{
		conn->setContext(HttpContext());
	}
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
	Buffer* buf,
	Timestamp receiveTime)
{
	HttpContext* context = any_cast<HttpContext>(conn->getMutableContext());

	if (!context->parseRequest(buf, receiveTime))
	{
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
	}

	if (context->gotAll())
	{
		onRequest(conn, context->request());
		context->reset();
	}
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
	const string& connection = req.getHeader("Connection");
	bool close = connection == "close" ||
		(req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
	HttpResponse response(close);
	httpCallBack_(req, &response);
	Buffer buf;
	response.appendToBuffer(&buf);
	conn->send(&buf);
	if (response.closeConnection())
	{
		conn->shutdown();
	}
}

