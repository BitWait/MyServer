#pragma once

#include <arpa/inet.h>

struct tcp_info;

namespace net{

	class Socket
	{
	public:
		explicit Socket(int fd)
			:sockfd_(fd)
		{

		}
		~Socket();
		int fd()const{ return sockfd_; }

		bool getTcpInfo(struct tcp_info*)const;
		bool getTcpInfoString(char* buf, int len)const;


	private:
		const int sockfd_;
	};

	namespace sockets{
		void toIpPort(char* buf, size_t size, const struct sockaddr_in& addr);
		void toIp(char* buf, size_t size, const struct sockaddr_in& addr);
		void fromIpPort(const char* ip, uint16_t port,
		struct sockaddr_in* addr);

	}
}