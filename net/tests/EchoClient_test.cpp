
#include "../eventloop.h"
#include "../../base/logging.h"
#include "../tcpclient.h"
#include "../inetaddress.h"
#include "../../base/singleton.h"
#include "../eventloopthreadpool.h"

#include <thread>
#include <utility>
#include <stdio.h>
#include <unistd.h>

using namespace net;

class EchoClient
{
 public:
  EchoClient(EventLoop* loop, const InetAddress& listenAddr, const string& id)
    : loop_(loop),
      client_(loop, listenAddr, id)
  {
    client_.setConnectionCallBack(
		std::bind(&EchoClient::onConnection, this, std::placeholders::_1));
    client_.setMessageCallBack(
		std::bind(&EchoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //client_.enableRetry();
  }

  void connect()
  {
    client_.connect();
  }
  // void stop();

 private:
  void onConnection(const TcpConnectionPtr& conn)
  {
    LOG_TRACE << conn->localAddress().toIpPort() << " -> "
        << conn->peerAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");

    conn->send("world\n");
  }

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
  {
    string msg(buf->retrieveAllAsString());
    LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
    if (msg == "quit\n")
    {
      conn->send("bye\n");
      conn->shutdown();
    }
    else if (msg == "shutdown\n")
    {
      loop_->quit();
    }
    else
    {
      conn->send(msg);
    }
  }

  EventLoop* loop_;
  TcpClient client_;
};

int main(void)
{
  LOG_INFO << "pid = " << getpid();
 
    EventLoop loop;
    InetAddress serverAddr(20000);

	EchoClient client(&loop, serverAddr, "EchoClient");
	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();
	client.connect();
    loop.loop();
}

