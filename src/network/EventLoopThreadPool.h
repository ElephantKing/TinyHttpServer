#ifndef TINY_EVENTLOOPTHREADPOOL_H
#define TINY_EVENTLOOPTHREADPOOL_H

#include <functional>
#include <memory>
#include <vector>
#include "../base/StringPiece.h"

namespace tiny {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
public:
	typedef std::function<void(EventLoop*)> ThreadInitCallback;

	EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg);
	EventLoopThreadPool(const EventLoopThreadPool&) = delete;
	~EventLoopThreadPool();
	
	void setThreadNum(int numThreads) { numThreads_ = numThreads; }
	
	void start(const ThreadInitCallback& callback = ThreadInitCallback());

	EventLoop* getNextLoop();

	EventLoop* getLoopForHash(size_t hashCode);

	std::vector<EventLoop*> getAllLoops();

	bool started() const {
		return started_;
	}

	const string& name() const {
		return name_;	
	}

private:
	EventLoop* baseLoop_;
	string name_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};

}//namespace tiny
#endif

