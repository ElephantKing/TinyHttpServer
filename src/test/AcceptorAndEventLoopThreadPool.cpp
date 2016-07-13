#include "Thread.h"
#include "ThreadPool.h"
#include "Mutex.h"
#include "Condition.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "EventLoopThread.h"
#include <iostream>
#include <string>
#include <cassert>
#include <map>
#include <arpa/inet.h>
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>


using namespace std;
using namespace std::placeholders;
using namespace tiny;

namespace {

int const kPort = 9527;
map<int, Channel*> connMap; //保存Channel*，用于结束时释放内存

void newConnectionCallback(EventLoop* loop, int listenFd) {
	loop->assertInLoopThread();
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
    int connFd = accept(listenFd, (struct sockaddr*)&client_addr, &length);
    if (connFd < 0)
    {
		loop->runInLoop(std::bind([]() {
			fprintf(stderr, "bind error: %d\n", errno);
		}));
		return ;
	} else {
		fprintf(stderr, "[%s:%d] connected\n", 
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	}
	
	Channel* connChannel = new Channel(loop, connFd);
	connMap[connFd] = connChannel;
	
	connChannel->setReadCallback(std::bind([=]() {
		loop->assertInLoopThread();
		char buff[2048];
		int n = ::read(connFd, buff, 2048);
		if (n != 0) {
			fprintf(stderr, "received %d bytes from [%s:%d]\n", 
				n, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		} else {
			loop->assertInLoopThread();
			fprintf(stderr, "[%s:%d] disconnected\n" ,
				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			::close(connChannel->fd());
		}
	}));
	
	connChannel->setCloseCallback(std::bind([=]() {
		loop->assertInLoopThread();
		connChannel->disableAll();
		connChannel->remove();
		delete connMap[connFd];
		connMap.erase(connFd);
	}));

	connChannel->enableReading();
}

void connectAndWrite() {
	int connFd = socket(AF_INET, SOCK_STREAM, 0);  
	if(-1 == connFd)  
	{  
		fprintf(stderr, "create socket fail ! \n");  
		return;  
	}
	sockaddr_in host;
	memset(&host, 0, sizeof(host));
	host.sin_family = AF_INET;
	host.sin_port = htons(kPort);
	host.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	if(-1 == connect(connFd,(struct sockaddr *)(&host), sizeof(struct sockaddr)))  
	{  
		fprintf(stderr, "connect fail !\r\n");  
		return;  
	}
	const char * buf = "hello, asshole";
	::write(connFd, buf, sizeof(buf));
	close(connFd);
}

}//unnamed namespace

int main() {
	//保证 EventLoop 最后一个析构
	EventLoopThread eventLoopThread;
	EventLoop* loop = eventLoopThread.startLoop();
	loop->runInLoop(std::bind([]() { assert(!CurrentThread::isMainThread()); }));
	assert(CurrentThread::isMainThread);

	auto listenAddress = InetAddress(kPort);
	Acceptor acceptor(loop, listenAddress, false);
	acceptor.setNewConnectionCallback(std::bind(newConnectionCallback, loop, _1));
	loop->runInLoop([&]() {
		acceptor.listen();
	});
	
	ThreadPool clientThread("clientThread");
	clientThread.setMaxQueueSize(100);
	clientThread.start(10);
	for (int i = 0; i < 20; ++i) {
		clientThread.run(connectAndWrite);
	}
	connectAndWrite();
	return 0;
}
