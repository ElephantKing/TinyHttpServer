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

void static sleep_1(unsigned int msecs)
{
  struct timeval tval;
  tval.tv_sec=msecs/1000;
  tval.tv_usec=(msecs*1000)%1000000;
  select(0,NULL,NULL,NULL,&tval);
}

int main() {

	cout << CurrentThread::t_threadName << endl;
	EventLoop loop;
	std::function<void()> checkNotInMainThread = [] () { assert(!CurrentThread::isMainThread()); };
	std::function<void()> checkInMainThread = [] () { assert(CurrentThread::isMainThread()); };
	std::function<void()> runInThread = [&] () { 
		checkNotInMainThread();
		for (int i = 0; i < 10; ++i) {
			loop.runInLoop(checkInMainThread);
			sleep_1(500);
		}
	};
	Thread childThread(runInThread, "childThread");
	loop.loop();
	childThread.start();
	childThread.join();
	return 0;
}
