#ifndef TINY_TCPCONNECTION_H
#define TINY_TCPCONNECTION_H

#include "../base/StringPiece.h"
#include "Callbacks.h"
#include "Buffer.h"
#include "InetAddress.h"

#include <memory>
#include <boost/any.hpp>
#include <atomic>

namespace tiny {

class Channel;
class EventLoop;
class Socket;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(EventLoop* loop,
				  const string& name,
				  int sockfd,
				  const InetAddress& localAddr,
				  const InetAddress& peerAddr);
	TcpConnection(const TcpConnection&) = delete;
	~TcpConnection();

	EventLoop* getLoop() const { return loop_; }
	const string& name() const { return name_; }
	const InetAddress& localAddress() const { return localAddr_; }
	const InetAddress& peerAddress() const { return peerAddr_; }
	bool connected() const { return state_ == kConnected; }
	bool disconnected() const { return state_ == kDisconnected; }

//	void send(string&& message);
	void send(const void* message, int len);
	void send(const StringPiece& message);
	void send(Buffer* message);
	//void send(Buffer&& message);
	void shutdown();
	void forceClose();
	void forceCloseWithDelay(double seconds);
	void setTcpNoDelay(bool on);
	void startRead();
	void stopRead();
	bool isReading() const { return reading_; }

	void setContext(const boost::any& context) { 
		context_ = context; 
	}

	const boost::any& getContext() const {
		return context_;
	}

	boost::any& getMutableContext() {
		return context_;
	}

	void setConnectionCallback(const ConnectionCallback& callback) {
		connectionCallback_ = callback;
	}

	void setMessageCallback(const MessageCallback& callback) {
		messageCallback_ = callback;
	}

	void setHighWaterMarkCallback(const HighWaterMarkCallback& callback, size_t highWaterMark) {
		highWaterMarkCallback_ = callback;
		highWaterMark_ = highWaterMark;
	}

	void setWriteCompleteCallback(const WriteCompleteCallback& callback) {
		writeCompleteCallback_ = callback;
	}

	Buffer* inputBuffer() {
		return &inputBuffer_;
	}

	Buffer* outputBuffer() {
		return &outputBuffer_;
	}

	void setCloseCallback(const CloseCallback& callback) {
		closeCallback_ = callback;
	}

	void connectEstablished();
	void connectDestroyed();

private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
//	void sendInLoop(string&& message);
	void sendInLoop(const StringPiece& message);
	void sendInLoop(const void* message, size_t len);
	void shutdownInLoop();
	void forceCloseInLoop();
	void setState(StateE s) { state_ = s; }
	const char* stateToString() const;
	void startReadInLoop();
	void stopReadInLoop();

	EventLoop* loop_;
	const string name_;
	std::atomic<StateE> state_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	const InetAddress localAddr_;
	const InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	HighWaterMarkCallback highWaterMarkCallback_;
	CloseCallback closeCallback_;
	size_t highWaterMark_;
	Buffer inputBuffer_;
	Buffer outputBuffer_;
	boost::any context_;
	bool reading_;
	// FIXME creationTime_, lastReceiveTime_, butesReceived_, bytesSent_
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

}// namespace tiny

#endif

