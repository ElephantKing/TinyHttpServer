#include "Thread.h"
#include "ThreadPool.h"
#include "Mutex.h"
#include "Condition.h"
#include "EventLoop.h"
#include "Channel.h"
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
using namespace tiny;

namespace {

int const kPort = 9527;
map<int, Channel*> connMap; //保存Channel*，用于结束时释放内存

int getListenFd(int port) {
	//监听地址
	sockaddr_in host;
	memset(&host, 0, sizeof(host));
	host.sin_family = AF_INET;
	host.sin_port = htons(port);
	host.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int listenFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(listenFd>= 0);	
	
	if(::bind(listenFd, (struct sockaddr *)&host, sizeof(host)))
    {
		fprintf(stderr, "bind error: %d\n", errno);
		return -1;
    }
    if(::listen(listenFd, SOMAXCONN))
    {

		fprintf(stderr, "listen error: %d\n", errno);
        return -1;
    }

	return listenFd;
}

void newConnectionCallback(EventLoop* loop, int listenFd) {
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
	std::function<void()> closeCallback = std::bind([=](){
		loop->assertInLoopThread();
		fprintf(stderr, "[%s:%d] clear\n" ,
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		loop->assertInLoopThread();
		connChannel->disableAll();
		connChannel->remove();
		loop->queueInLoop([loop, connFd] () {
			delete connMap[connFd];
		});
		connMap.erase(connFd);
		::close(connFd);

	});
	
	connChannel->setReadCallback(std::bind([=]() {
		loop->assertInLoopThread();
		char buff[2048];
		int n = ::read(connFd, buff, 2048);
		if (n != 0) {
			fprintf(stderr, "received %d bytes from [%s:%d]\n", 
				n, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		} else {
		//socket为阻塞socket，读到0时已经为半关闭
		//直接closeCallback（），不再等到下次poll，因为可能半关闭也会
		//一直被判定为可读
			loop->assertInLoopThread();
			fprintf(stderr, "[%s:%d] disconnected\n" ,
				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			closeCallback();
		}
	}));
	
	//poller保证执行CloseCallback时是安全的
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
	
	if(-1 == connect(connFd,(struct sockaddr *)(&host), sizeof(struct sockaddr)))  
	{  
		fprintf(stderr, "connect fail !\r\n");  
		return;  
	}
	const char * buf = "hello, asshole";
	::write(connFd, buf, sizeof(buf));
	::close(connFd);
}

}//unnamed namespace

int main() {
	EventLoop* loop;
	std::function<void()> runInLoopThread = [&] () {
		EventLoop loopInChildThread;
		loop = &loopInChildThread;
		loopInChildThread.loop();
	};
	Thread thread(runInLoopThread, "loopThread");
	thread.start();
	CurrentThread::sleepMsec(2000); //waiting for loop start, should use condition replace sleep
	loop->runInLoop(std::bind([]() { assert(!CurrentThread::isMainThread()); }));
	assert(CurrentThread::isMainThread());
	//loop->assertInLoopThread();

	auto listenFd = getListenFd(kPort);
	std::function<void(Timestamp)> onNewConnection = std::bind(newConnectionCallback, loop, listenFd);

	//Channel 的所有操作都应该在loopthread，这样就不需要对channel加锁
	Channel* listenChannel_ptr = new Channel(loop, listenFd);
	Channel& listenChannel = *listenChannel_ptr;
	loop->runInLoop([&](){
		listenChannel.setReadCallback(onNewConnection);
		listenChannel.enableReading();
	});
	
	ThreadPool clientThread("clientThread");
	clientThread.setMaxQueueSize(100);
	clientThread.start(2);
	for (int i = 0; i < 10; ++i) {
		clientThread.run(connectAndWrite);
	}
	
	fprintf(stderr, "work dipatch completed, sleeping and wait...\n");
	CurrentThread::sleepMsec(1000 * 1000);
	listenChannel.disableAll();
	listenChannel.remove();
	delete listenChannel_ptr;
	loop->quit();
	thread.join();
	clientThread.stop();
	return 0;
}

