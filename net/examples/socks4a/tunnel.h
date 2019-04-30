#ifndef EXAMPLES_SOCKS4A_TUNNEL_H
#define EXAMPLES_SOCKS4A_TUNNEL_H

#include "../../../base/logging.h"
#include "../../eventloop.h"
#include "../../inetaddress.h"
#include "../../tcpclient.h"
#include "../../tcpserver.h"
#include "../../anyone.h"

class Tunnel : public std::enable_shared_from_this<Tunnel>
{
 public:
  Tunnel(net::EventLoop* loop,
         const net::InetAddress& serverAddr,
         const net::TcpConnectionPtr& serverConn)
    : client_(loop, serverAddr, serverConn->name()),
      serverConn_(serverConn)
  {
    LOG_INFO << "Tunnel " << serverConn->peerAddress().toIpPort()
             << " <-> " << serverAddr.toIpPort();
  }

  ~Tunnel()
  {
    LOG_INFO << "~Tunnel";
  }

  void setup()
  {
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    client_.setConnectionCallBack(
        std::bind(&Tunnel::onClientConnection, shared_from_this(), _1));
    client_.setMessageCallBack(
        std::bind(&Tunnel::onClientMessage, shared_from_this(), _1, _2, _3));
    serverConn_->setHighWaterMarkCallBack(
        std::bind(&Tunnel::onHighWaterMarkWeak,
                  std::weak_ptr<Tunnel>(shared_from_this()), kServer, _1, _2),
        1024*1024);
  }

  void connect()
  {
    client_.connect();
  }

  void disconnect()
  {
    client_.disconnect();
    // serverConn_.reset();
  }

 private:
  void teardown()
  {
    client_.setConnectionCallBack(net::defaultConnectionCallBack);
    client_.setMessageCallBack(net::defaultMessageCallBack);
    if (serverConn_)
    {
      serverConn_->setContext(any());
      serverConn_->shutdown();
    }
    clientConn_.reset();
  }

  void onClientConnection(const net::TcpConnectionPtr& conn)
  {
    using std::placeholders::_1;
    using std::placeholders::_2;

    LOG_DEBUG << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
      conn->setTcpNoDelay(true);
      conn->setHighWaterMarkCallBack(
          std::bind(&Tunnel::onHighWaterMarkWeak,
                    std::weak_ptr<Tunnel>(shared_from_this()), kClient, _1, _2),
          1024*1024);
      serverConn_->setContext(conn);
      serverConn_->startRead();
      clientConn_ = conn;
      if (serverConn_->inputBuffer()->readableBytes() > 0)
      {
        conn->send(serverConn_->inputBuffer());
      }
    }
    else
    {
      teardown();
    }
  }

  void onClientMessage(const net::TcpConnectionPtr& conn,
                       net::Buffer* buf,
                       Timestamp)
  {
    LOG_DEBUG << conn->name() << " " << buf->readableBytes();
    if (serverConn_)
    {
      serverConn_->send(buf);
    }
    else
    {
      buf->retrieveAll();
      abort();
    }
  }

  enum ServerClient
  {
    kServer, kClient
  };

  void onHighWaterMark(ServerClient which,
                       const net::TcpConnectionPtr& conn,
                       size_t bytesToSent)
  {
    using std::placeholders::_1;

    LOG_INFO << (which == kServer ? "server" : "client")
             << " onHighWaterMark " << conn->name()
             << " bytes " << bytesToSent;

    if (which == kServer)
    {
      if (serverConn_->outputBuffer()->readableBytes() > 0)
      {
        clientConn_->stopRead();
        serverConn_->setWriteCompleteCallBack(
            std::bind(&Tunnel::onWriteCompleteWeak,
                      std::weak_ptr<Tunnel>(shared_from_this()), kServer, _1));
      }
    }
    else
    {
      if (clientConn_->outputBuffer()->readableBytes() > 0)
      {
        serverConn_->stopRead();
        clientConn_->setWriteCompleteCallBack(
            std::bind(&Tunnel::onWriteCompleteWeak,
                      std::weak_ptr<Tunnel>(shared_from_this()), kClient, _1));
      }
    }
  }

  static void onHighWaterMarkWeak(const std::weak_ptr<Tunnel>& wkTunnel,
                                  ServerClient which,
                                  const net::TcpConnectionPtr& conn,
                                  size_t bytesToSent)
  {
    std::shared_ptr<Tunnel> tunnel = wkTunnel.lock();
    if (tunnel)
    {
      tunnel->onHighWaterMark(which, conn, bytesToSent);
    }
  }

  void onWriteComplete(ServerClient which, const net::TcpConnectionPtr& conn)
  {
    LOG_INFO << (which == kServer ? "server" : "client")
             << " onWriteComplete " << conn->name();
    if (which == kServer)
    {
      clientConn_->startRead();
      serverConn_->setWriteCompleteCallBack(net::WriteCompleteCallBack());
    }
    else
    {
      serverConn_->startRead();
      clientConn_->setWriteCompleteCallBack(net::WriteCompleteCallBack());
    }
  }

  static void onWriteCompleteWeak(const std::weak_ptr<Tunnel>& wkTunnel,
                                  ServerClient which,
                                  const net::TcpConnectionPtr& conn)
  {
    std::shared_ptr<Tunnel> tunnel = wkTunnel.lock();
    if (tunnel)
    {
      tunnel->onWriteComplete(which, conn);
    }
  }

 private:
  net::TcpClient client_;
  net::TcpConnectionPtr serverConn_;
  net::TcpConnectionPtr clientConn_;
};
typedef std::shared_ptr<Tunnel> TunnelPtr;

#endif  // MUDUO_EXAMPLES_SOCKS4A_TUNNEL_H
