#ifndef _TINY_EVENTLOOPTHREAD_H_
#define _TINY_EVENTLOOPTHREAD_H_


#include "../base/Condition.h"
#include "../base/Mutex.h"
#include "../base/Thread.h"

namespace tiny {

class EventLoop;

class EventLoopThread {
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;

	EventLoopThread(const EventLoopThread&) = delete;
	EventLoopThread(const ThreadInitCallback& callback = ThreadInitCallback(),
			        const string& name = string());

	~EventLoopThread();
	EventLoop* startLoop();

private:
	void threadFunc();

	EventLoop* loop_;
	bool exiting_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
	ThreadInitCallback callback_;
};


}// namespace tiny
#endif

