#include "EventLoopThread.h"
#include "EventLoop.h"

namespace tiny {

EventLoopThread::EventLoopThread(const ThreadInitCallback& threadInitCallback,
								 const string& name)
	: loop_(nullptr),
	  exiting_(false),
	  thread_(std::bind(&EventLoopThread::threadFunc, this), name),
	  mutex_(),
	  cond_(mutex_),
	  callback_(threadInitCallback)
{
}

EventLoopThread::~EventLoopThread() {
	exiting_ = true;
	if (loop_ != nullptr) {
		loop_->quit();
		thread_.join();
	}
}

EventLoop* EventLoopThread::startLoop() {
	assert(!thread_.started_());
	thread_.start();

	{
		MutexLockGuard lock(mutex_);
		while (loop_ == nullptr) {
			cond_.wait();
		}
	}
	
	return loop_;
}

void EventLoopThread::threadFunc() {
	EventLoop loop;
	if (callback_) {
		callback_(&loop);
	}

	{
		MutexLockGuard lock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}

	loop.loop();
	loop_ = nullptr;
}

} //namespace tiny

