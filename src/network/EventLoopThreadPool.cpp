#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <functional>
#include <cstdio>

namespace tiny {


EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg)
	: baseLoop_(baseLoop),
	  name_(nameArg),
	  started_(false),
	  numThreads_(0),
	  next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::start(const ThreadInitCallback& callback) {
	assert(!started_);
	baseLoop_->assertInLoopThread();

	started_ = true;

	for (int i = 0; i < numThreads_; ++i) {
		char buf[name_.size() + 32];
		snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
		EventLoopThread* t = new EventLoopThread(callback, buf);
		loops_.push_back(t->startLoop());
	}
	
	if (numThreads_ == 0 && callback) {
		callback(baseLoop_);
	}
}

EventLoop* EventLoopThreadPool::getNextLoop() {
	baseLoop_->assertInLoopThread();
	assert(started_);
	EventLoop* loop = baseLoop_;

	if (!loops_.empty()) {
		loop = loops_[next_];
		++next_;
		if (static_cast<size_t>(next_) >= loops_.size()) {
			next_ = 0;	
		}
	}
	return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode) {
	baseLoop_->assertInLoopThread();	
	EventLoop* loop = baseLoop_;
	
	if (!loops_.empty()) {
		loop = loops_[hashCode % loops_.size()];
	}
	return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
	baseLoop_->assertInLoopThread();
	assert(started_);
	if (loops_.empty()) {
		return std::vector<EventLoop*>(1, baseLoop_);
	} else {
		return loops_;
	}
}

}// namespace tiny

