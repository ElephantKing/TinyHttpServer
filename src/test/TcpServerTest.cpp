
/**
 *       echo  server
 */

#include "TcpServer.h"
#include "EventLoopThread.h"
#include "Channel.h"
#include "Socket.h"
#include "SocketsOps.h"
#include "EventLoop.h"
#include <string>
#include <map>


using namespace tiny;
using namespace std::placeholders;
using namespace std;

namespace {
int16_t listenPort = 9527;

void onMessage(const TcpConnectionPtr& conn, tiny::Buffer* buff, Timestamp) {
	string msg = buff->retrieveAllAsString();
	cout << "Echo Server: Received: " << msg.size() << "bytes from [" << conn->peerAddress().toIp()
		 << ":" << conn->peerAddress().toPort() << "]" << endl;
	conn->send(msg);
}

void onConnection(const TcpConnectionPtr& conn) {
	cout << "Echo Server: [" << conn->peerAddress().toIp() << ":" << conn->peerAddress().toPort() << "] -> "
		 << "[" << conn->localAddress().toIp() << ":" << conn->localAddress().toPort() << "]"
		 << (conn->connected() ? "connected" : "disconnected") << endl;
}

void sendToServer(int fd) {
	for (int i = 0; i < 10; ++i) {
	cout << "send start" << endl;
		char buff[] = "hello, server!\n";
		int n = ::write(fd, buff, sizeof(buff));
		if (n <= 0) {
			cout << "write err:" << errno << " ," << strerror(errno) << endl;
		}
		CurrentThread::sleepMsec(1000);
	}
}

}

int main() {
	EventLoopThread loopThread([](EventLoop*){ assert(!CurrentThread::isMainThread()); }, "LoopThread");
	EventLoop *loop = loopThread.startLoop();
	TcpServer* server_ptr = new TcpServer(loop, InetAddress(listenPort), "echoServer", TcpServer::kNoReusePort);
	TcpServer& echoServer = *server_ptr;
	echoServer.setThreadNum(0);
	echoServer.setMessageCallback(std::bind(onMessage, _1, _2, _3));
	echoServer.setConnectionCallback(std::bind(onConnection, _1));
	loop->runInLoop([&](){
		echoServer.start();
	});

	InetAddress serverAddr("127.0.0.1", listenPort);
	Socket clientSock(sockets::createNonblockingOrDie(serverAddr.family()));
	Channel* channel_ptr = new Channel(loop, clientSock.fd());
	sockets::connect(clientSock.fd(), serverAddr.getSockAddr());
	channel_ptr->setReadCallback([&](Timestamp){
		char buff[1024];
		int n = ::read(clientSock.fd(), buff, sizeof(buff));
		cout << "Echo Client: received" << n << "bytes from server ---  " << buff << endl;
	});
	loop->runInLoop([&](){
		channel_ptr->enableReading();
	});

	cout << "waiting server ready" << endl;
	CurrentThread::sleepMsec(2 * 1000);
	Thread clientThread(std::bind(sendToServer, clientSock.fd()));
	clientThread.start();
	clientThread.join();

	cout << "waiting receive compelete.." << endl;
	CurrentThread::sleepMsec(1 * 1000);
	loop->runInLoop([=](){
		channel_ptr->disableAll();
		channel_ptr->remove();
		delete channel_ptr;
		delete server_ptr;
		loop->quit();
	});
	return 0;
}
