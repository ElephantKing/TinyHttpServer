#ifndef _TINY_SOCKETOPS_H_
#define _TINY_SOCKETOPS_H_

#include <arpa/inet.h>

namespace tiny {
	
namespace sockets {

int createNonblockingOrDie(sa_family_t family);

int connect(int sockfd, const struct sockaddr* addr);
void bindOrDie(int sockfd);
void listenOrDie(int sockfd);
int accept(int sockfd);
ssize_t read(int sockfd, void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, size_t iovcnt);
ssize_t write(int sockfd, const void*buf, size_t count);
void close(int sockfd);
void shutdownWrite(int sockfd);

void toIpPort(char *buf, size_t size,
		      const struct sockaddr* addr);
void toIp(char *buf, size_t size,
		  const struct sockaddr* addr);

void fromIpPort(const char *op, uint16_t port,
		        struct sockaddr_in *addr); 
void fromIpPort(const char *op, uint16_t port,
		        struct sockaddr_in6 *addr); 

int getSocketError(int sockfd);

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6 addr);
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

struct sockaddr_in6 getLocalAddr(int sockfd);
struct sockaddr_in6 getPeerAddr(int sockfd);
bool isSelfConnect(int sockfd);

}// namespace sockets
}// namespace tiny
#endif
