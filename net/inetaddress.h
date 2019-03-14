#pragma once

#include <netinet/in.h>
#include <inttypes.h>
#include <string>

namespace net{

	class InetAddress
	{
	public:
		explicit InetAddress(uint16_t port = 0, bool loopBackOnly = false);
		InetAddress(const std::string & ip, uint16_t len);
		InetAddress(const struct sockaddr_in& addr)
			:addr_(addr)
		{

		}

		std::string toIp()const;
		std::string toIpPort()const;
		uint16_t toPort()const;

		const struct sockaddr_in& getSockaddrInet()const{ return addr_; }
		void setSockAddrInet(const struct sockaddr_in& addr){ addr_ = addr;}

		uint32_t ipNetEndian() const { return addr_.sin_addr.s_addr; }
		uint16_t portNetEndian() const { return addr_.sin_port; }

		static bool resolve(const std::string& hostname, InetAddress* result);

	private:
		struct sockaddr_in addr_;
	};

}


