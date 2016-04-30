#include "Socket.h"

#include "InetAddress.h"
#include "SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <cstdio>

tiny::Socket::~Socket() {
	sockets::close(sockfd_);
}

bool tiny::Socket::getTcpInfo(struct tcp_info* tcpi) const {
	socklen_t len = sizeof(*tcpi);
	bzero(tcpi, len);
	return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}
