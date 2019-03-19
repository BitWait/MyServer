#include "../sockets.h"
#include "../inetaddress.h"

#include "../../base/logging.h"


using net::InetAddress;
using namespace net;

int main(void)
{
	char data[256];
	Socket sock(sockets::createNonblockingOrDie());
	
	char buf[256] = { 0 };
	sock.getTcpInfoString(buf, sizeof buf);

	LOG_INFO << buf;

	InetAddress addr(8888, true);
	
	//sock.setReuseAddr(true);
	//sock.setReusePort(true);
	sock.bindAddress(addr);
	sock.listen();
	InetAddress addr1;
	int connfd = sock.accept(&addr1);
	LOG_INFO << connfd;
	while (true)
	{
		memset(data, 0, sizeof data);
		int ret = sockets::read(connfd, data, sizeof(data));
		// 打印输出
		fputs(data, stdout);
		// 返回给客户端
		sockets::write(connfd, data, ret);
	}
	sockets::close(connfd);
}