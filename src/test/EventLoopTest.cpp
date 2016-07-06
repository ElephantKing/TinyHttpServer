#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"
#include "EventLoop.h"
#include <iostream>
#include <string>
#include <cassert>
#include <functional>


using namespace std;
using namespace tiny;

namespace {

MutexLock globalMutex;

void printThreadInfo() {
	MutexLockGuard lock(globalMutex);	
	fprintf(stderr, "Thread[%d:%ld]", ::getpid(), reinterpret_cast<intptr_t>(CurrentThread::tid()));
}

}

int main() {
	assert(CurrentThread::isMainThread());
	printThreadInfo();	
	fork();
	EventLoop loop;
	std::function<void()> checkNotInMainThread = [] () { printThreadInfo(); assert(!CurrentThread::isMainThread()); };
	std::function<void()> checkInMainThread = [] () {printThreadInfo(); assert(CurrentThread::isMainThread()); };
	std::function<void()> runInThread = [&] () { 
		checkNotInMainThread();
		for (int i = 0; i < 10; ++i) {
			loop.runInLoop(checkInMainThread);
		}
		CurrentThread::sleepMsec(5 * 1000);
		loop.runInLoop([&](){loop.quit();});
	};
	Thread childThread(runInThread, "childThread");
	childThread.start();
	fprintf(stderr, "loop start\n");
	loop.loop();
	childThread.join();
	return 0;
}
