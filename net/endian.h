#pragma once

#include <endian.h>
#include <stdint.h>

namespace net{
	
	namespace sockets{

#if defined(__clang__) || __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"

		inline uint64_t hostToNetwork64(uint64_t host64)
		{
			return htobe64(host64);
			//return host64;
		}

		inline uint32_t hostToNetwork32(uint32_t host32)
		{
			return htobe32(host32);
			//return host32;
		}

		inline uint16_t hostToNetwork16(uint16_t host16)
		{
			return htobe16(host16);
			//return host16;
		}

		inline uint64_t networkTohost64(uint64_t network64)
		{
			return be64toh(network64);
			//return network64;
		}

		inline uint32_t networkTohost32(uint32_t network32)
		{
			return be32toh(network32);
			//return network32;
		}

		inline uint16_t networkTohost16(uint16_t network16)
		{
			return be16toh(network16);
			//return network16;
		}

#if defined(__clang__) || __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#else
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wold-style-cast"
#endif
	}
}