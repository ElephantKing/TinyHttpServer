#ifndef TINY_INETADDRESS_H
#define TINY_INETADDRESS_H

#include <netinet/in.h>
#include <cstdlib>
#include "../base/StringPiece.h"

namespace tiny {

namespace sockets {

const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);

} // namespace sockets

class InetAddress {
public:
	explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

	InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);

	sa_family_t family() const { return addr_.sin_family; }

private:
	union {
		struct sockaddr_in addr_;
		struct sockaddr_in6 addr_6;
	};
};

} // namespace tiny
#endif

