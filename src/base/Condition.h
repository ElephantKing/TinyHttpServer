#ifndef _TINY_CONDITION_H_
#define _TINY_CONDITION_H_

#include "Mutex.h"

namespace tiny {

class Condition {

public:
	explicit Condition(MutexLock& mutex) 
		: mutex_(mutex) {
		pthread_cond_init(&pcond_, NULL);
	}

	~Condition() {
		pthread_cond_destroy(&pcond_);
	}

	void wait() {
		MutexLock::UnassignGuard ug(mutex_);
		pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
	}

	void notify() {
		pthread_cond_signal(&pcond_);
	}

	void notifyAll() {
		pthread_cond_broadcast(&pcond_);
	}
		
private:
	MutexLock& mutex_;
	pthread_cond_t pcond_;
};


}

#endif

