#include "sockets.h"
#include "../base/logging.h"
#include "endian.h"

#include <string.h>

#include <assert.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>

#include <sys/socket.h>
#include <unistd.h>

using namespace net;

void sockets::toIpPort(char* buf, size_t size, const struct sockaddr_in& addr)
{
	assert(size >= INET_ADDRSTRLEN);
	::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
	size_t end = ::strlen(buf);
	uint16_t port = sockets::networkTohost16(addr.sin_port);
	assert(size > end);
	snprintf(buf + end, size - end, ":%u", port);

}
void sockets::toIp(char* buf, size_t size, const struct sockaddr_in& addr)
{
	assert(size >= INET_ADDRSTRLEN);
	::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
}

void sockets::fromIpPort(const char* ip, uint16_t port,
struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = htobe16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
		LOG_SYSERR << "sockets::fromIpPort";
	}
}