#ifndef _TINY_THREADPOOL_H_
#define _TINY_THREADPOOL_H_

#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"

#include <functional>
#include <string>
#include <vector>
#include <deque>
#include <memory>

namespace tiny {

using std::string;
using std::unique_ptr;

class ThreadPool {
public:
	typedef std::function<void()> Task;

	explicit ThreadPool(const string& name = string("ThreadPool"));
	ThreadPool(const ThreadPool&) = delete;
	
	~ThreadPool();

	void setMaxQueueSize(int maxSize) { MaxQueueSize_ = maxSize; }
	void setThreadInitCallback(const Task& callback) { threadInitCallback_ = callback; }

	void start(int numThreads);
	void stop();
 
	const string& name() const {
		return name_;
	}

	size_t queueSize() const;

	void run(const Task& task);
	void run(const Task&& task);

private:
	bool isFull() const;
	void runInThread();
	Task take();

	mutable MutexLock mutex_;
	Condition notEmpty_;
	Condition notFull_;
	Task threadInitCallback_;
	std::vector<unique_ptr<tiny::Thread>> threads_;
	std::deque<Task> queue_;
	string name_;
	size_t MaxQueueSize_;
	bool running_;
};


}// namespace tiny
#endif

