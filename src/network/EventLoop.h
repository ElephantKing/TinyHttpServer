#ifndef _TINY_EVENTLOOP_H_
#define _TINY_EVENTLOOP_H_

#include <vector>
#include <iostream>
#include <functional>
#include <memory>
#include <boost/any.hpp>
#include <atomic>

#include "../base/Mutex.h"
#include "../base/CurrentThread.h"
#include "../base/Timestamp.h"

#include "Callbacks.h"
#include "TimerId.h"

namespace tiny {

class Channel;
class Poller;
class TimerQueue;

class EventLoop {
public:
	typedef std::function<void()> Functor;

	EventLoop();
	EventLoop(const EventLoop&) = delete;
	~EventLoop();

	void loop();

	void quit();

	Timestamp pollReturnTime() const { return poolReturnTime_; }

	int64_t ineration() const { return iteration_; }  //what's meanning?

	void runInLoop(const Functor& callback);
	void queueInLoop(const Functor& callback);

	void runInLoop(Functor&& callback);
	void queueInLoop(Functor&& callback);

	TimerId runAt(const Timestamp& time, const TimerCallback& callback);
	TimerId runAfter(double delay, const TimerCallback& callback);
	TimerId runEvery(double interval, const TimerCallback& callback);

	TimerId runAt(const Timestamp& time, const TimerCallback&& callback);
	TimerId runAfter(double delay, const TimerCallback&& callback);
	TimerId runEvery(double interval, const TimerCallback&& callback);

	void cancel(TimerId timerId);

	void wakeup();
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);
	bool hasChannel(Channel* channel);

	void assertInLoopThread() {
		if (!isInLoopThread()) {
			abortNotInLoopThread();
		}
		assert(isInLoopThread());
	}

	bool isInLoopThread() const {  
		return threadId_ == CurrentThread::tid();
	}
	
	bool eventHandling() const { return eventHandling_; }

	void setContext(const boost::any& context) 
	{ context_ = context; }

	const boost::any* getContext() const 
	{ return &context_; }

	static EventLoop* getEventLoopOfCurrentThread();

private:
	void abortNotInLoopThread();
	void handleRead();
	void doPendingFunctors();

	void printActiveChannels() const;

	typedef std::vector<Channel*> ChannelList;

	std::atomic<bool> looping_;
	std::atomic<bool> quit_;
	std::atomic<bool> eventHandling_;
	std::atomic<bool> callingPendingFunctors_;

	int64_t iteration_; //循环次数
	const pthread_t threadId_;
	Timestamp poolReturnTime_;
	std::unique_ptr<Poller> poller_;
	std::unique_ptr<TimerQueue> timerQueue_;
	int wakeupFd_[2];

	std::unique_ptr<Channel> wakeupChannel_;
	boost::any context_;

	ChannelList activeChannels_;
	Channel* currentActiveChannel_;
	
	MutexLock mutex_;
	std::vector<Functor> doPendingFunctors_;

};

}// namespace tiny
#endif

