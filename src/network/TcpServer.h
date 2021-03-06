#ifndef TINY_TCPSERVER_H
#define TINY_TCPSERVER_H

#include <atomic>
#include "TcpConnection.h"

#include <map>
#include <memory>

namespace tiny {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer {
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;
	enum Option {
		kNoReusePort,
		kReusePort
	};

	TcpServer(EventLoop* loop,
			  const InetAddress& listenAddr,
			  const string& nameArg,
			  Option option = kNoReusePort);

	~TcpServer();

	const string& ipPort() const { return ipPort_; }
	const string& name() const { return name_; }
	EventLoop* getLoop() const { return loop_; }

	void setThreadNum(int numThreads);
	void setThreadInitCallback(const ThreadInitCallback& callback) {
		threadInitCallback_ = callback;
	}

	void start();

	void setConnectionCallback(const ConnectionCallback& callback) {
		connectionCallback_ = callback;
	}

	void setMessageCallback(const MessageCallback& callback) {
		messageCallback_ = callback;
	}

private:
	void newConnection(int sockfd, const InetAddress& peerAddr);

	void removeConnection(const TcpConnectionPtr& conn);

	void removeConnectionInLoop(const TcpConnectionPtr& conn);

	typedef std::map<string, TcpConnectionPtr> ConnectionMap;

	EventLoop* loop_;
	const string ipPort_;
	const string name_;
	std::unique_ptr<Acceptor> acceptor_;
	std::shared_ptr<EventLoopThreadPool> threadPool_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	ThreadInitCallback threadInitCallback_;
	std::atomic<int32_t> started_;
	int nextConnId_;
	ConnectionMap connections_;
};


}//namespace tiny
#endif

