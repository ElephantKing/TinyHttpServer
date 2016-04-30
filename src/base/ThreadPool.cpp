#include "ThreadPool.h"
#include "Exception.h"

#include <functional>

namespace tiny {

ThreadPool::ThreadPool(const string& name)
	: mutex_(),
	  notEmpty_(mutex_),
	  notFull_(mutex_),
	  name_(name),
	  MaxQueueSize_(0),
	  running_(false)
{
}

ThreadPool::~ThreadPool() {
	if (running_) {
		stop();
	}
}

void ThreadPool::start(int numThreads) {
	assert(threads_.empty());
	running_ = true;
	threads_.reserve(numThreads);
	for (int i = 0; i < numThreads; ++i) {
		char id[32];
		snprintf(id, sizeof(id), "%d", i + 1);
		threads_.push_back(std::unique_ptr<tiny::Thread>(new Thread(std::bind(&ThreadPool::runInThread, this), name_+id)));
		threads_[i]->start();
	}
	if (numThreads == 0 && threadInitCallback_) {
		threadInitCallback_();
	}
}

void ThreadPool::stop() {

	{
		MutexLockGuard lock(mutex_);
		running_ = false;
		notEmpty_.notifyAll();
	}
	for(auto it = threads_.begin(); it != threads_.end(); ++it) {
		(*it)->join();	
	}
}

size_t ThreadPool::queueSize() const {
	MutexLockGuard lock(mutex_);
	return queue_.size();
}

void ThreadPool::run(const Task &task) {
	if (threads_.empty()) { //如果没有线程可用，主线程来跑task
		task();
	} else {
		MutexLockGuard lock(mutex_);
		while(isFull()) {
			notFull_.wait();
		}
		assert(!isFull());

		queue_.push_back(task);
		notEmpty_.notify();
	}
}

void ThreadPool::run(const Task &&task) {
	if (threads_.empty()) {
		task();
	} else {
		MutexLockGuard lock(mutex_);
		while(isFull()) {
			notFull_.wait();
		}
		assert(!isFull());

		queue_.push_back(std::move(task));
		notEmpty_.notify();
	}
}

ThreadPool::Task ThreadPool::take() {
	//只会由Thread 在 runInThread 中调用
	MutexLockGuard lock(mutex_);
	while(queue_.empty() && running_) { //只有队列空并且在running才wait，否则直接取
		notEmpty_.wait();
	}

	Task task;
	if (!queue_.empty()) {  // if not running_, 取走的task为空任务，线程终止
		task = queue_.front();
		queue_.pop_front();
		if (MaxQueueSize_ > 0) {  // 如果最大任务数量为0，不通知notFull
			notFull_.notify();
		}
	}
	return task;
}

bool ThreadPool::isFull() const {
	mutex_.assertLocked();  //只有获取锁之后才可以调用这个函数, 函数为private，只供ThreadPool内部使用
	return MaxQueueSize_ > 0 && queue_.size() >= MaxQueueSize_;
}


void ThreadPool::runInThread() {
	try {
		if (threadInitCallback_) {
			threadInitCallback_();
		}
		while (running_) {
			Task task(take());
			if (task) {
				task(); //假如调用了stop，并且queue_为空，那么就会取到一个空的task,同时也可以让用户随便run
			}
		}
	} catch(const Exception& ex) {
		fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason %s\n", ex.what());
		fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
		abort();
	} catch (const std::exception& ex) {
		fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason %s\n", ex.what());
		abort();
	} catch (...) {
		fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
		throw;
	}
}
}// namespace tiny

