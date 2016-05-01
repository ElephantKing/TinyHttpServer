#ifndef _TINY_SOCKET_H_
#define _TINY_SOCKET_H_

struct tcp_info;

namespace tiny {

class InetAddress;

class Socket {

public:
	Socket(const Socket&) = delete;
	explicit Socket(int sockfd) 
		: sockfd_(sockfd)
	{  }

	~Socket();
	int fd() { return sockfd_; }
	bool getTcpInfo(struct tcp_info*) const;
	bool getTcpInfoString(char *buf, int len) const;

	void bindAddress(const InetAddress& localaddr);

	void listen();

	int accept(InetAddress* peerAddr);

	void shutdownWrite();

	// disable Nagle's algorithm
	void setTcpNoDelay(bool on);

	// disable SO_REUSERADDR
	void setReuseAddr(bool on);

	// SO_REUSEPORT
	void setReusePort(bool on);

	// SO_KEEPALIVE
	void setKeepAlive(bool on);

private:
	const int sockfd_;
};

}//namsepace tiny
#endif
