#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "SocketsOps.h"

#include <functional>
#include <cstdio>

namespace tiny {

using namespace std::placeholders;
TcpServer::TcpServer(EventLoop* loop,
					 const InetAddress& listenAddr,
					 const string& nameArg,
					 Option option)
	: loop_(loop),
	  ipPort_(listenAddr.toIpPort()),
	  name_(nameArg),
	  acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
	  threadPool_(new EventLoopThreadPool(loop, name_)),
	  connectionCallback_(defaultConnectionCallback),
	  messageCallback_(defaultMessageCallback),
	  nextConnId_(1)
{
	acceptor_->setNewConnectionCallback(
			std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
	loop_->assertInLoopThread();
//	LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

	for (auto it = connections_.begin(); it != connections_.end(); ++it) {
		TcpConnectionPtr conn = it->second;
		it->second.reset();
		conn->getLoop() ->runInLoop(
				std::bind(&TcpConnection::connectDestroyed, conn));
		conn.reset();
	}
}

void TcpServer::setThreadNum(int numThreads) {
	assert(0 <= numThreads);
	threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
	if (!started_) {
		threadPool_->start(threadInitCallback_);

		assert(!acceptor_->listenning());
		loop_->runInLoop(
				std::bind(&Acceptor::listen, acceptor_.get()));
	}
}

void TcpServer::newConnection(int sockfd, const  InetAddress& peerAddr) {
	loop_->assertInLoopThread();
	EventLoop* ioLoop = threadPool_->getNextLoop();
	char buf[64];
	snprintf(buf, sizeof(buf), "-%s#%d", ipPort_.c_str(), nextConnId_);
	++nextConnId_;
	string connName = name_ + buf;

//	LOG_INFO << "TcpServer::newConnection [" << name_
//			 << "] - new connection [" << connName
//			 << "] from " << peerAddr.toIpPort();
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	//FIXME poll with zero timeout to double confirm th new connection
	//FIXME use make_shared if necessary
	TcpConnectionPtr conn(new TcpConnection(
				ioLoop,
				connName,
				sockfd,
				localAddr,
				peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback( //FIXME unsafe
			std::bind(&TcpServer::removeConnection, this, _1));
	ioLoop->runInLoop(
			std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
	//FIXME unsafe
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
	loop_->assertInLoopThread();
//	LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
//			 << "] - connection " << conn->name();
	size_t n = connections_.erase(conn->name());
	(void)n;
	assert(n == 1);
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queueInLoop(
			std::bind(&TcpConnection::connectDestroyed, conn));
}

}//namspace tine
