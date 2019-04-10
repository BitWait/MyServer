// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef NET_HTTP_HTTPSERVER_H
#define NET_HTTP_HTTPSERVER_H


#include "../tcpserver.h"

namespace net
{

	class HttpRequest;
	class HttpResponse;

	/// A simple embeddable HTTP server designed for report status of a program.
	/// It is not a fully HTTP 1.1 compliant server, but provides minimum features
	/// that can communicate with HttpClient and Web browser.
	/// It is synchronous, just like Java Servlet.
	class HttpServer
	{
	public:
		typedef std::function<void(const HttpRequest&,
			HttpResponse*)> HttpCallBack;

		HttpServer(EventLoop* loop,
			const InetAddress& listenAddr,
			const string& name,
			TcpServer::Option option = TcpServer::kNoReusePort);

		EventLoop* getLoop() const { return server_.getLoop(); }

		/// Not thread safe, callback be registered before calling start().
		void setHttpCallBack(const HttpCallBack& cb)
		{
			httpCallBack_ = cb;
		}

		void start();

	private:
		void onConnection(const TcpConnectionPtr& conn);
		void onMessage(const TcpConnectionPtr& conn,
			Buffer* buf,
			Timestamp receiveTime);
		void onRequest(const TcpConnectionPtr&, const HttpRequest&);

		TcpServer server_;
		HttpCallBack httpCallBack_;
	};

}  // namespace net


#endif  // MUDUO_NET_HTTP_HTTPSERVER_H
