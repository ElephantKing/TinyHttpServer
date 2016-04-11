#ifndef _TINY_THREAD_H_
#define _TINY_THREAD_H_

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
using std::shared_ptr;
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
	shared_ptr<pid_t> tid_;
	ThreadFunc func_;
	string name_;

	static std::atomic<int> numCreated_;
};




}
#endif

