#ifndef _TINY_CURRENTTHREAD_H_
#define _TINY_CURRENTTHREAD_H_

#include <cstdint>
#include <unistd.h>
#include <pthread.h>

namespace tiny {
namespace CurrentThread {

extern thread_local pthread_t t_cachedTid;
extern thread_local char t_tidString[64];
extern thread_local const char* t_threadName;

void cacheTid();

inline pthread_t tid() {
	if(t_cachedTid == 0) {
		cacheTid();
	}
	return t_cachedTid;
}

bool isMainThread();

void sleepMsec(size_t msec);

}//namespace CurrentThread
}//namespace tiny

#endif

