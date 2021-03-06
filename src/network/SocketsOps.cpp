#include "SocketsOps.h"
#include "Endian.h"

#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

namespace tiny {

namespace sockets {

const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr) {
	return static_cast<const struct sockaddr*>(static_cast<const void*>(addr));
}

struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr) {
	return static_cast<struct sockaddr*>(static_cast<void*>(addr));
}


void shutdownWrite(int sockfd)
{
  if (::shutdown(sockfd, SHUT_WR) < 0)
  {
//    LOG_SYSERR << "sockets::shutdownWrite";
  }
}

const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr) {
	return static_cast<const struct sockaddr_in*>(static_cast<const void*>(addr));
}


const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr) {
	return static_cast<const struct sockaddr_in6*>(static_cast<const void*>(addr));
}

int createNonblockingOrDie(sa_family_t family) {
#if VALGRIND
	int sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		//LOG_SYSFATAL << "sockets::createNonblockingOrDie";
	}

	setNonBlockAndCloseOnExec(sockfd);
#else
	int sockfd = ::socket(family, SOCK_STREAM /*| SOCK_NONBLOCK | SOCK_CLOEXEC*/, IPPROTO_TCP);
	if (sockfd < 0) {
		//LOG_SYSFATAL << "sockets::createNonblockingOrDie";
		fprintf(stderr, "create socket failed, %d, %s\n", errno, strerror(errno));
	}
	assert(sockfd >= 0);
#endif
	return sockfd;
}

void setNonBlockAndCloseOnExec(int sockfd) {
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	assert(flags >= 0);
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_SETFL, flags);
	assert(ret == 0);
	flags = ::fcntl(sockfd, F_GETFD, 0);
	assert(flags >= 0);
	flags |= FD_CLOEXEC;
	ret = ::fcntl(sockfd, F_SETFD, flags);
	assert(ret == 0);
	(void)ret;
}

void bindOrDie(int sockfd, const struct sockaddr* addr) {
	int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
//	fprintf(stderr, "bind ok, addr:%s, port:%d", 
//				inet_ntoa(((sockaddr_in*)addr)->sin_addr), ((sockaddr_in*)addr)->sin_port);
	if (ret < 0) {
		//LOG_SYSFATAL << "sockets::bindOrDie";
		fprintf(stderr, "bind failed, err: %d, %s, addr:%s, port:%d", 
				errno, strerror(errno), inet_ntoa(((sockaddr_in*)addr)->sin_addr), ((sockaddr_in*)addr)->sin_port);
	}
	assert(ret >= 0);
}

void listenOrDie(int sockfd) {
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0) {
		//LOG_SYSFATAL << "sockets::listenOrDie";
	}
	assert(ret == 0);
}

int accept(int sockfd, struct sockaddr_in6* addr) {
	socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
	int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
	setNonBlockAndCloseOnExec(connfd);
	if (connfd < 0) {
		int savedErrno = errno;
//		LOG_SYSERR << "Socket::accept";
		switch (savedErrno) {
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO:
			case EMFILE:
				errno = savedErrno;
				break;
			case EBADF:
			case EFAULT:
			case ENFILE:
			case ENOBUFS:
			case ENOTSOCK:
			case EOPNOTSUPP:
				//LOG_FATAL << "unexpected error of ::accept" << savedErrno;
				break;
			default:
				//LOG_FATAL << "unknow error of ::accept" << savedErrno;
				break;
		}
	}
	return connfd;
}

int connect(int sockfd, const struct sockaddr* addr) {
	return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr)));
}

ssize_t read(int sockfd, void* buf, size_t count) {
	return ::read(sockfd, buf, count);
}

ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt)
{
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t write(int sockfd, const void*buf, size_t count) {
	return ::write(sockfd, buf, count);
}

void close(int sockfd) {
	if (::close(sockfd) < 0) {
		//LOG_SYSERR << "sockets::close";
	}
}

void toIpPort(char* buf, size_t size,
			  const struct sockaddr* addr)
{
	toIp(buf, size, addr);
	size_t end = strlen(buf);
	const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
	uint16_t port = sockets::networkToHost16(addr4->sin_port);
	assert(size > end);
	snprintf(buf + end, size - end, ":%u", port);
}

void toIp(char *buf, size_t size,
		  const struct sockaddr* addr)
{
	if (addr->sa_family == AF_INET) {
		assert(size >= INET_ADDRSTRLEN);
		const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
		::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
	} else if (addr->sa_family == AF_INET6) {
		assert(size >= INET6_ADDRSTRLEN);
		const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
		::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
	}
}

void fromIpPort(const char* ip, uint16_t port,
			    struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = hostToNetwork16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
//		LOG_SYSERR << "sockets::fromIpPort";
	}
}

void fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in6* addr)
{
  addr->sin6_family = AF_INET6;
  addr->sin6_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
  {
    //LOG_SYSERR << "sockets::fromIpPort";
  }
}

int getSocketError(int sockfd) {
	int optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		return errno;
	} else {
		return optval;
	}
}

struct sockaddr_in6 getLocalAddr(int sockfd) {
	struct sockaddr_in6 localaddr;
	memset(&localaddr, 0, sizeof(localaddr));
	socklen_t addrlen = static_cast<socklen_t>(sizeof (localaddr));
	if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
//		LOG_SYSERR << "sockets::getLocalAddr";
	}
	return localaddr;
}

struct sockaddr_in6 getPeerAddr(int sockfd) {
	struct sockaddr_in6 peeraddr;
	memset(&peeraddr, 0, sizeof(peeraddr));
	socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
	if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
		//LOG_SYSERR << "sockets::getPeerAddr";
	}
	return peeraddr;
}

bool isSelfconnect(int sockfd) {
	struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
	struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
	if (localaddr.sin6_family == AF_INET) {
		const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
		const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
		return laddr4->sin_port == raddr4->sin_port
			   && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
	} else if (localaddr.sin6_family == AF_INET6) {
		return localaddr.sin6_family == peeraddr.sin6_family
			   && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof(localaddr.sin6_addr)) == 0;
	} else {
		return false;
	}
}

}//namespace sockets
}//namespace tiny

