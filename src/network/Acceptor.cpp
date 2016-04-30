#include "Acceptor.h"

#include "EventLoop.h"

#include <functional>

#include <errno.h>
#include <fcntl.h>

tiny::Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
	: loop_(loop),
	  acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
	  acceptChannel_(loop, acceptSocket_.fd()),
	  listenning_(false),
	  idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) 
{
	assert(idleFd_ >= 0);
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reuseport);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallback(
			std::bind(&Acceptor::handleRead, this));
}

tiny::Acceptor::~Acceptor() {
	acceptChannel_.disableAll(); //为什么不能直接remove
	acceptChannel_.remove();
	::close(idleFd_);
}

void tiny::Acceptor::Acceptor::listen() {
	loop_->assertInLoopThread();
	listenning_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void tiny::Acceptor::handleRead() {
	loop_->assertInLoopThread();
	InetAddress peerAddr;
	//FIXME loop until no more
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0) {
		if (newConnectionCallback_) {
			newConnectionCallback_(connfd, peerAddr);
		} else {
			sockets::close(connfd);
		}
	} else {
//		LOG_SYSER << "in Acceptor::handleRead";	
		if (errno == EMFILE) {
			::close(idleFd_);
			idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}
