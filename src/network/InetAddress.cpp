#include "InetAddress.h"

#include "Endian.h"
#include "SocketsOps.h"

#include <netdb.h>
#include <cstring>
#include <assert.h>
#include <netinet/in.h>

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

namespace tiny {

//static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6), "address Error");
//static_assert(offsetof(sockaddr_in, sin_family) == 0, "offset");
//static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "offset6");
//static_assert(offsetof(sockaddr_in, sin_port) == 2, "offset port");
//static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "offset port6");

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6) {
	if (ipv6) {
		memset(&addr6_, 0, sizeof(addr6_));
		addr6_.sin6_family = AF_INET6;
		in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
		addr6_.sin6_addr = ip;
		addr6_.sin6_port = sockets::hostToNetwork16(port);
	} else {
		memset(&addr_, 0, sizeof(addr_));
		addr_.sin_family = AF_INET;
		in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
		addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
		addr_.sin_port = sockets::hostToNetwork16(port);
	}
}

InetAddress::InetAddress(StringArg ip, uint16_t port, bool ipv6) {
	if (ipv6) {
		memset(&addr6_, 0, sizeof(addr6_));
		sockets::fromIpPort(ip.c_str(), port, &addr6_);
	} else {
		memset(&addr_, 0, sizeof(addr_));
		sockets::fromIpPort(ip.c_str(), port, &addr_);
	}
}

string InetAddress::toIpPort() const {
	char buf[64] = "";
	sockets::toIpPort(buf, sizeof(buf), getSockAddr());
	return buf;
}

string InetAddress::toIp() const {
	char buf[64] = "";
	sockets::toIp(buf, sizeof(buf), getSockAddr());
	return buf;
}

uint32_t InetAddress::ipNetEndian() const  {
	assert(family() == AF_INET);
	return addr_.sin_addr.s_addr;
}

uint16_t InetAddress::toPort() const {
	return sockets::networkToHost16(portNetEndian());
}

static thread_local  char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(StringArg /*hostname*/, InetAddress* /* out */) {
	//assert(out != nullptr);
	//struct hostent hent;
	//struct hostent* he = nullptr;
	//int herrno = 0;
	//memset(&hent, 0, sizeof(hent));
	return true;
}

}//namespace tiny
