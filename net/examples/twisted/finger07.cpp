#include "../../eventloop.h"
#include "../../tcpserver.h"
#include "../../../base/singleton.h"
#include "../../eventloopthreadpool.h"

#include <map>
using namespace net;

typedef std::map<string, string> UserMap;
UserMap users;


string getUser(const string& user)
{
	string result = "No such user";
	UserMap::iterator it = users.find(user);
	if (it != users.end())
	{
		result = it->second;
	}
	return result;
}

void onMessage(const TcpConnectionPtr& conn,
	Buffer* buf,
	Timestamp receiveTime)
{
	const char* crlf = buf->findCRLF();
	if (crlf)
	{
		string user(buf->peek(), crlf);
		conn->send(getUser(user) + "\r\n");
		buf->retrieveUntil(crlf + 2);
		conn->shutdown();
	}
}


int main(void)
{
	users["level"] = "happy and well";
	EventLoop loop;
	TcpServer server(&loop, InetAddress(1079), "Finger");
	server.setMessageCallback(onMessage);
	Singleton<EventLoopThreadPool>::Instance().Init(&loop, 0);
	Singleton<EventLoopThreadPool>::Instance().start();

	server.start();
	loop.loop();
}