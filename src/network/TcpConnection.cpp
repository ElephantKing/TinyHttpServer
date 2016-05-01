#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketsOps.h"

#include <errno.h>

namespace tiny {

using namespace std::placeholders;

void defaultConnectionCallback(const TcpConnectionPtr& conn) {
	(void)conn;
//	LOG_TRACE << conn->localAddress().toIpPort() << " -> "
//			  << conn->peerAddress.toIpPort() << " is "
//			  << (conn->connected() ? "UP" : "DOWN");
}

void defaultMessageCallback(const TcpConnectionPtr&,
							Buffer* buf,
							Timestamp)
{
	buf->retrieveAll();
}

TcpConnection::TcpConnection(EventLoop *loop,
							 const string& nameArg,
							 int sockfd,
							 const InetAddress& localAddr,
							 const InetAddress& peerAddr)
	: loop_(loop),
	  name_(nameArg),
	  state_(kConnecting),
	  socket_(new Socket(sockfd)),
	  channel_(new Channel(loop, sockfd)),
	  localAddr_(localAddr),
	  peerAddr_(peerAddr),
	  highWaterMark_(64 * 1024 * 1024),
	  reading_(true)
{
	channel_->setReadCallback(
			std::bind(&TcpConnection::handleRead, this, _1));	
	channel_->setWriteCallback(
			std::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(
			std::bind(&TcpConnection::handleError, this));
//	LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
//			  << " fd=" << sockfd;
	socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
//	LOG_DEBUG << "TcpConnection::dtorl[" << name_ << "] at " << this
//			  << " fd=" << channel_->fd()
//			  << " state=" << stateToString();
	assert(state_ == kDisconnected);
}

bool TcpConnection::getTcpInfo(struct tcp_info* tcpi) const {
	return socket_->getTcpInfo(tcpi);
}

string TcpConnection::getTcpInfoString() const {
	char buf[1024];
	buf[0] = '\0';
	socket_->getTcpInfoString(buf, sizeof(buf));
	return buf;
}

void TcpConnection::send(const StringPiece& message) {
	if (state_ == kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(message);
		} else {
			std::bind( 
				[this, &message] () { 
					this->sendInLoop(message.as_string()); 
				});
		}
	}
}



}//namespace tiny

