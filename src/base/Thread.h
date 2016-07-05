#ifndef TINY_THREAD_H
#define TINY_THREAD_H

#include "CurrentThread.h"

#include <cstdint>
#include <pthread.h>
#include <unistd.h>
#include <functional>
#include <string>
#include <memory>
#include <atomic>

namespace tiny {

using std::string;

class Thread {
public:
	typedef std::function<void()> ThreadFunc;

	explicit Thread(const ThreadFunc&, const string& name = string());

	explicit Thread(const ThreadFunc&&, const string& name = string());

	Thread(const Thread&) = delete;

	~Thread();

	void start();

	int join();

	bool started() const { return started_; }

private:
	void setDefaultName();

	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	std::shared_ptr<pthread_t> tid_;
	ThreadFunc func_;
	string name_;

	static std::atomic<int> numCreated_;
};




}
#endif

