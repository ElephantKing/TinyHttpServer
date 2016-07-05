#ifndef _TINY_MUTEX_H_
#define _TINY_MUTEX_H_

#include "CurrentThread.h"

#include <pthread.h>
#include <cassert>

namespace tiny {

class MutexLock {
public:
	MutexLock() : holder_(0) {
		pthread_mutex_init(&mutex_, NULL);
	}

	~MutexLock() { 
		assert(holder_ == 0);
		pthread_mutex_destroy(&mutex_); 
	}

	MutexLock(const MutexLock&) = delete;

	bool isLockedByThisThread() const {
		return holder_ == CurrentThread::tid();
	}

	void lock() {
		pthread_mutex_lock(&mutex_);
		assignHolder();
	}

	void assertLocked() const {
		void assert(isLockedByThisThread());
	}
	void unlock() {
		unassignHolder();
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t* getPthreadMutex() {
		return &mutex_;
	}

private:
	friend class Condition;
	void unassignHolder() {
		holder_ = 0;
	}

	void assignHolder() {
		holder_ = CurrentThread::tid();
	}

	class UnassignGuard {
	public:
		explicit UnassignGuard(MutexLock& owner)
			: owner_(owner) {
				owner.unassignHolder();
		}

		UnassignGuard(const UnassignGuard&) = delete;

		~UnassignGuard() {
			owner_.assignHolder();
		}

	private:
		MutexLock& owner_;
	};

	pthread_t holder_;
	pthread_mutex_t mutex_;
};

class MutexLockGuard {
public:
	MutexLockGuard(const MutexLockGuard&) = delete;

	explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) {
		mutex_.lock();
	}

	~MutexLockGuard() {
		mutex_.unlock();
	}

private:
	MutexLock& mutex_;
};

}

#endif

