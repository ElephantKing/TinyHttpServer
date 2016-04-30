#ifndef TINY_CONNECTOR_H
#define TINY_CONNECTOR_H

#include "InetAddress.h"

#include <memory>
#include <functional>

namespace tiny {

class Channel;
class EventLoop;

class Connector {
public:
	typedef std::function<void(int sockfd)> NewConnectionCallback;

	Connector(const Connector&) = delete;
	~Connector();

private:
	enum States { KDisconnected, KConnecting, KConnected };
	static const int kMaxRetryDelayMs = 30 * 1000;
	static const int kInitRetryDelayMs = 500;

};


}//namespace tiny
#endif

