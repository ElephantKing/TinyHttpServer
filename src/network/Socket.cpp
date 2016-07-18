#include "Socket.h"

#include "InetAddress.h"
#include "SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <strings.h>
#include <arpa/inet.h>
#include <cstdio>

namespace tiny {
Socket::~Socket() {
	sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& addr)
{
  sockets::bindOrDie(sockfd_, addr.getSockAddr());
}

void Socket::listen()
{
  sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
  struct sockaddr_in6 addr;
  bzero(&addr, sizeof addr);
  int connfd = sockets::accept(sockfd_, &addr);
  if (connfd >= 0)
  {
    peeraddr->setSockAddrInet6(addr);
  }
  return connfd;
}

void Socket::shutdownWrite()
{
  sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0 && on)
  {
//    LOG_SYSERR << "SO_REUSEPORT failed.";
  }
#else
  if (on)
  {
    LOG_ERROR << "SO_REUSEPORT is not supported.";
  }
#endif
}

void Socket::setKeepAlive(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
}

}//namespace tiny
