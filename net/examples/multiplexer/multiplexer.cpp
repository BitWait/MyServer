#include "../../../base/logging.h"
#include "../../eventloop.h"
#include "../../inetaddress.h"
#include "../../tcpclient.h"
#include "../../tcpserver.h"
#include "../../../base/singleton.h"
#include "../../eventloopthreadpool.h"

#include <queue>
#include <utility>
#include <atomic>
#include <mutex>
#include <stdio.h>
#include <unistd.h>

using namespace net;

const int kMaxConns = 10;  // 65535
const size_t kMaxPacketLen = 255;
const size_t kHeaderLen = 3;

const uint16_t kClientPort = 3333;
const char* backendIp = "127.0.0.1";
const uint16_t kBackendPort = 9999;

class MultiplexServer
{
 public:
  MultiplexServer(EventLoop* loop,
                  const InetAddress& listenAddr,
                  const InetAddress& backendAddr,
                  int numThreads)
    : server_(loop, listenAddr, "MultiplexServer"),
      backend_(loop, backendAddr, "MultiplexBackend"),
      numThreads_(numThreads),
      oldCounter_(0),
      startTime_(Timestamp::now())
  {
	  using std::placeholders::_1;
	  using std::placeholders::_2;
	  using std::placeholders::_3;
    server_.setConnectionCallBack(
        std::bind(&MultiplexServer::onClientConnection, this, _1));
    server_.setMessageCallBack(
        std::bind(&MultiplexServer::onClientMessage, this, _1, _2, _3));

    backend_.setConnectionCallBack(
        std::bind(&MultiplexServer::onBackendConnection, this, _1));
    backend_.setMessageCallBack(
        std::bind(&MultiplexServer::onBackendMessage, this, _1, _2, _3));
    backend_.enableRetry();

    // loop->runEvery(10.0, std::bind(&MultiplexServer::printStatistics, this));

  }

  void start()
  {
    LOG_INFO << "starting " << numThreads_ << " threads.";
    backend_.connect();
    server_.start();
  }

 private:
  void sendBackendPacket(int id, Buffer* buf)
  {
    size_t len = buf->readableBytes();
    assert(len <= kMaxPacketLen);
    uint8_t header[kHeaderLen] = {
      static_cast<uint8_t>(len),
      static_cast<uint8_t>(id & 0xFF),
      static_cast<uint8_t>((id & 0xFF00) >> 8)
    };
    buf->prepend(header, kHeaderLen);
    TcpConnectionPtr backendConn;
    {
		std::lock_guard<std::mutex> lck(mutex_);
      backendConn = backendConn_;
    }
    if (backendConn)
    {
      backendConn->send(buf);
    }
  }

  void sendBackendString(int id, const string& msg)
  {
    assert(msg.size() <= kMaxPacketLen);
    Buffer buf;
    buf.append(msg);
    sendBackendPacket(id, &buf);
  }

  void sendBackendBuffer(int id, Buffer* buf)
  {
    while (buf->readableBytes() > kMaxPacketLen)
    {
      Buffer packet;
      packet.append(buf->peek(), kMaxPacketLen);
      buf->retrieve(kMaxPacketLen);
      sendBackendPacket(id, &packet);
    }
    if (buf->readableBytes() > 0)
    {
      sendBackendPacket(id, buf);
    }
  }

  void sendToClient(Buffer* buf)
  {
    while (buf->readableBytes() > kHeaderLen)
    {
      int len = static_cast<uint8_t>(*buf->peek());
      if (buf->readableBytes() < len + kHeaderLen)
      {
        break;
      }
      else
      {
        int id = static_cast<uint8_t>(buf->peek()[1]);
        id |= (static_cast<uint8_t>(buf->peek()[2]) << 8);

        TcpConnectionPtr clientConn;
        {
		  std::lock_guard<std::mutex> lck(mutex_);
          std::map<int, TcpConnectionPtr>::iterator it = clientConns_.find(id);
          if (it != clientConns_.end())
          {
            clientConn = it->second;
          }
        }
        if (clientConn)
        {
          clientConn->send(buf->peek() + kHeaderLen, len);
        }
        buf->retrieve(len + kHeaderLen);
      }
    }
  }

  void onClientConnection(const TcpConnectionPtr& conn)
  {
    LOG_TRACE << "Client " << conn->peerAddress().toIpPort() << " -> "
        << conn->localAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
      int id = -1;
      {
		 std::lock_guard<std::mutex> lck(mutex_);
        if (!availIds_.empty())
        {
          id = availIds_.front();
          availIds_.pop();
          clientConns_[id] = conn;
        }
      }

      if (id <= 0)
      {
        conn->shutdown();
      }
      else
      {
        conn->setContext(id);
        char buf[256];
        snprintf(buf, sizeof(buf), "CONN %d FROM %s IS UP\r\n", id,
                 conn->peerAddress().toIpPort().c_str());
        sendBackendString(0, buf);
      }
    }
    else
    {
      if (!conn->getContext().empty())
      {
        int id = any_cast<int>(conn->getContext());
        assert(id > 0 && id <= kMaxConns);
        char buf[256];
        snprintf(buf, sizeof(buf), "CONN %d FROM %s IS DOWN\r\n",
                 id, conn->peerAddress().toIpPort().c_str());
        sendBackendString(0, buf);

		std::lock_guard<std::mutex> lck(mutex_);
        if (backendConn_)
        {
          availIds_.push(id);
          clientConns_.erase(id);
        }
        else
        {
          assert(availIds_.empty());
          assert(clientConns_.empty());
        }
      }
    }
  }

  void onClientMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
  {
    size_t len = buf->readableBytes();
	transferred_.fetch_add(len);
	receivedMessages_.fetch_add(1);
    transferred_.fetch_add(len);
    receivedMessages_.fetch_add(1);
    if (!conn->getContext().empty())
    {
      int id = any_cast<int>(conn->getContext());
      sendBackendBuffer(id, buf);
      // assert(buf->readableBytes() == 0);
    }
    else
    {
      buf->retrieveAll();
      // FIXME: error handling
    }
  }

  void onBackendConnection(const TcpConnectionPtr& conn)
  {
    LOG_TRACE << "Backend " << conn->localAddress().toIpPort() << " -> "
              << conn->peerAddress().toIpPort() << " is "
              << (conn->connected() ? "UP" : "DOWN");
    std::vector<TcpConnectionPtr> connsToDestroy;
    if (conn->connected())
    {
		std::lock_guard<std::mutex> lck(mutex_);
      backendConn_ = conn;
      assert(availIds_.empty());
      for (int i = 1; i <= kMaxConns; ++i)
      {
        availIds_.push(i);
      }
    }
    else
    {
		std::lock_guard<std::mutex> lck(mutex_);
      backendConn_.reset();
      connsToDestroy.reserve(clientConns_.size());
      for (std::map<int, TcpConnectionPtr>::iterator it = clientConns_.begin();
          it != clientConns_.end();
          ++it)
      {
        connsToDestroy.push_back(it->second);
      }
      clientConns_.clear();
      while (!availIds_.empty())
      {
        availIds_.pop();
      }
    }

    for (std::vector<TcpConnectionPtr>::iterator it = connsToDestroy.begin();
        it != connsToDestroy.end();
        ++it)
    {
      (*it)->shutdown();
    }
  }

  void onBackendMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
  {
    size_t len = buf->readableBytes();
    transferred_.fetch_add(len);
    receivedMessages_.fetch_add(1);
    sendToClient(buf);
  }

  void printStatistics()
  {
    Timestamp endTime = Timestamp::now();
    int64_t newCounter = transferred_.load();
    int64_t bytes = newCounter - oldCounter_;
    int64_t msgs = receivedMessages_.exchange(0);
    double time = timeDifference(endTime, startTime_);
    printf("%4.3f MiB/s %4.3f Ki Msgs/s %6.2f bytes per msg\n",
        static_cast<double>(bytes)/time/1024/1024,
        static_cast<double>(msgs)/time/1024,
        static_cast<double>(bytes)/static_cast<double>(msgs));

    oldCounter_ = newCounter;
    startTime_ = endTime;
  }

  TcpServer server_;
  TcpClient backend_;
  int numThreads_;
  std::atomic<int64_t> transferred_;
  std::atomic<int64_t> receivedMessages_;
  int64_t oldCounter_;
  Timestamp startTime_;
  std::mutex mutex_;
  TcpConnectionPtr backendConn_;
  std::map<int, TcpConnectionPtr> clientConns_;
  std::queue<int> availIds_ ;
};

int main(int argc, char* argv[])
{
  printf("pid=%d, tid=%d", getpid(), std::this_thread::get_id());
  int numThreads = 4;
  if (argc > 1)
  {
    backendIp = argv[1];
  }
  if (argc > 2)
  {
    numThreads = atoi(argv[2]);
  }
  EventLoop loop;
  InetAddress listenAddr(kClientPort);
  InetAddress backendAddr(backendIp, kBackendPort);
  MultiplexServer server(&loop, listenAddr, backendAddr, numThreads);

  server.start();

  loop.loop();
}
