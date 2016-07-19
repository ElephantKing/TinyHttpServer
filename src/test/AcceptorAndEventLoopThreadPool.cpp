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

void newConnectionCallback(EventLoop* loop, int connFd, InetAddress peerAddr) {
	loop->assertInLoopThread();
	Channel* connChannel = new Channel(loop, connFd);
	connMap[connFd] = connChannel;
	
	std::function<void()> closeCallback = std::bind([=](){
		loop->assertInLoopThread();
		connChannel->disableAll();
		connChannel->remove();
		loop->queueInLoop([=](){
			delete connMap[connFd];
		});
		connMap.erase(connFd);
		::close(connChannel->fd());
	});

	connChannel->setReadCallback(std::bind([=]() {
		loop->assertInLoopThread();
		char buff[2048];
		int n = ::read(connFd, buff, 2048);
		if (n != 0) {
			fprintf(stderr, "received %d bytes from [%s:%d]\n", 
				n, peerAddr.toIp().c_str(), peerAddr.toPort());
		} else {
			loop->assertInLoopThread();
			fprintf(stderr, "[%s:%d] disconnected\n" ,
				peerAddr.toIp().c_str(), peerAddr.toPort());
			closeCallback();
		}
	}));
	
	connChannel->setCloseCallback(closeCallback);

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
	
	if(connect(connFd,(struct sockaddr *)(&host), sizeof(struct sockaddr)) < 0)  
	{  
		fprintf(stderr, "connect fail! err:%d %s\n", errno, strerror(errno));  
		return;  
	}
	const char * buf = "hello, asshole";
	::write(connFd, buf, sizeof(buf));
	::close(connFd);
}

}//unnamed namespace

int main() {
	//保证 EventLoop 最后一个析构
	EventLoopThread eventLoopThread;
	EventLoop* loop = eventLoopThread.startLoop();
	loop->runInLoop(std::bind([]() { 
				assert(!CurrentThread::isMainThread()); 
				fprintf(stderr, "loop started\n");
	}));
	assert(CurrentThread::isMainThread());

	auto listenAddress = InetAddress(kPort);
	Acceptor *acceptor_ptr = new Acceptor(loop, listenAddress, false);
	Acceptor& acceptor = *acceptor_ptr;
	acceptor.setNewConnectionCallback(std::bind(newConnectionCallback, loop, _1, _2));
	loop->runInLoop([&]() {
		acceptor.listen();
	});
	
	ThreadPool clientThread("clientThread");
	//等待listen开始。。
	CurrentThread::sleepMsec(20 * 1000);
	fprintf(stderr, "start write\n");
	clientThread.setMaxQueueSize(100);
	clientThread.start(1);
	for (int i = 0; i < 10; ++i) {
		clientThread.run(connectAndWrite);
	}
	connectAndWrite();
	//等待connectAndWrite结束
	fprintf(stderr, "wait to close.....\n");
	CurrentThread::sleepMsec(200 * 1000);
	loop->runInLoop([=]() {
		delete acceptor_ptr;
	});
	return 0;
}
