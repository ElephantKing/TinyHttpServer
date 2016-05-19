#include "InetAddress.h"

#include "Endian.h"
#include "SocketsOps.h"

#include <netdb.h>
#include <cstring>
#include <netinet/in.h>

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

namespace tiny {

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6), "address Error");
static_assert(offsetof(sockaddr_in, sin_family) == 0, "offset");
static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "offset6");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "offset port");
static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "offset port6");
}//namespace tiny
