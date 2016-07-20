
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

map<int, Channel*> channelMap;
map<int, Socket*> sockMap;

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
		char buff[] = "hello, server!\n";
		::write(fd, buff, sizeof(buff));
		CurrentThread::sleepMsec(1000);
	}
}

}

int main() {
	EventLoopThread loopThread([](EventLoop*){ assert(!CurrentThread::isMainThread()); }, "LoopThread");
	EventLoop *loop = loopThread.startLoop();
	TcpServer echoServer(loop, InetAddress(listenPort), "echoServer", TcpServer::kNoReusePort);
	echoServer.setThreadNum(0);
	echoServer.setMessageCallback(std::bind(onMessage, _1, _2, _3));
	echoServer.setConnectionCallback(std::bind(onConnection, _1));

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
	channel_ptr->enableReading();

	Thread clientThread(std::bind(sendToServer, clientSock.fd()));
	clientThread.start();

	cout << "waiting to exit...." << endl;
	CurrentThread::sleepMsec(30 * 1000);
	loop->runInLoop([=](){
		delete channel_ptr;
	});
	return 0;
	
}
