#ifndef _TINY_CURRENTTHREAD_H_
#define _TINY_CURRENTTHREAD_H_

#include <cstdint>
#include <unistd.h>

namespace tiny {
namespace CurrentThread {

/*FIXME: using c++11 thread_local keyword*/
extern __thread int64_t t_cachedTid;
extern __thread char t_tidString[64];
extern __thread const char* t_threadName;

void cacheTid();

inline pid_t tid() {
	if(t_cachedTid == 0) {
		cacheTid();
	}
	return t_cachedTid;
}

bool isMainThread();

}
}

#endif

