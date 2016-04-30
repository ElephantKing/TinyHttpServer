#ifndef _TINY_ACCEPTOR_H_
#define _TINY_ACCEPTOR_H_

#include <functional>

#include "Channel.h"

namespace tiny {

class EventLoop;
class InetAddress;

class Acceptor {
public:
	typedef std::function<void (int sockfd,
						   const InetAddress&)> NewConnectionCallback;

	Acceptor(const Acceptor&) = delete;
	Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
	~Acceptor();

	void setNewConnectionCallback(const NewConnectionCallback& callback) {
		newConnectionCallback_ = callback;
	}

	bool listenning() const { return listenning_; }
	void listen();

private:
	void handleRead();

	EventLoop* loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallback newConnectionCallback_;
	bool listenning_;
	int idleFd_;
};

}//namespace tiny
#endif
