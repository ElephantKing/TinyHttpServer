#include "Thread.h"
#include "CurrentThread.h"
#include "Exception.h"


#include <memory>
#include <cstdlib>
#include <cassert>

namespace tiny {

namespace CurrentThread {

using std::weak_ptr;

__thread int64_t t_cachedTid = 0;

__thread char t_tidString[64];

__thread const char* t_threadName = "unknown";

void cacheTid() {
	if (t_cachedTid == 0) {
		/// 这里使用pthread_self, 在mac os 下这只能保证同一个进程下的不同线程
		/// 的tid 不同，在Linux下使用gettid这个调度id作为tid更好
		t_cachedTid = reinterpret_cast<int64_t>(pthread_self());
	}
}

void afterFork() {
	CurrentThread::t_cachedTid = 0;
	CurrentThread::t_threadName = "main";
}

class ThreadNameInitializer {
public:	
	ThreadNameInitializer() : mainThreadId(0) {
		// 这里的代码只会在main函数之前调用一次
		t_threadName = "main";
		mainThreadId = CurrentThread::tid(); // 记录mainThread 的tid
		pthread_atfork(NULL, NULL, &afterFork);
	}
	int64_t mainThreadId;
};


ThreadNameInitializer init;     //全局变量，初始化在main() 函数执行之前

struct ThreadData {
	typedef tiny::Thread::ThreadFunc ThreadFunc;
	ThreadFunc func_;
	string name_;
	weak_ptr<pid_t> wkTid_;

	ThreadData(ThreadFunc&& func,
			   const string& name,
			   const shared_ptr<pid_t>& tid) 
		: func_(std::move(func)),
		  name_(name),
		  wkTid_(tid)  {  }


	void runInThread() {
		pid_t tid = 0;
		tid = CurrentThread::tid();
	
		shared_ptr<pid_t> ptid = wkTid_.lock();
		if (ptid) {
			//回调，将Thread对象的tid 设置成当前线程的tid
			//由于Thread对象可能已经被析构，使用weak_ptr
			*ptid = tid;
			ptid.reset();
		}

		CurrentThread::t_threadName = name_.empty() ? "nomarlThread" : name_.c_str();
//		::prctl(PR_SET_NAME, CurrentThread::t_threadName);
	
		try {
			func_();
			CurrentThread::t_threadName = "finished";
		} catch (const Exception& ex) {
			CurrentThread::t_threadName = "crashed";
			fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
			fprintf(stderr, "reason : %s\n", ex.what());
			fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
			abort();
		} catch (const std::exception& ex) {
			CurrentThread::t_threadName = "crashed";
			fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
			fprintf(stderr, "reason : %s\n", ex.what());
			abort();
		} catch (...) {
			CurrentThread::t_threadName = "crashed";
			fprintf(stderr, "unknown reason in Thread %s\n", name_.c_str());
			throw;
		}
	}
};

void *startThread(void *obj) {
	ThreadData* data = static_cast<ThreadData*>(obj);
	data->runInThread();
	delete data;
	return NULL;
}

bool isMainThread() {
	return tid() == init.mainThreadId;
}

}// namespace CurrentThread

std::atomic<int> Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string& n) 
	: started_(false),
	  joined_(false),
	  pthreadId_(0),
	  tid_(new pid_t(0)),
	  func_(func),
	  name_(n)
{
	setDefaultName();	
}

Thread::Thread(const ThreadFunc&& func, const string& n) 
	: started_(false),
	  joined_(false),
	  pthreadId_(0),
	  tid_(new pid_t(0)),
	  func_(func),
	  name_(n)
{
	setDefaultName();	
}

Thread::~Thread() {
	if (started_ && !joined_) {
		pthread_detach(pthreadId_);
	}
}

void Thread::setDefaultName() {
	int num = numCreated_;
	if (name_.empty()) {
		char buf[32];
		snprintf(buf, sizeof(buf), "Thread%d", num);
		name_ = buf;
	}
}

void Thread::start() {
	assert(!started_);
	started_ = true;
	CurrentThread::ThreadData *data = new CurrentThread::ThreadData(std::move(func_), name_, tid_); 
	if (pthread_create(&pthreadId_, NULL, &CurrentThread::startThread, data)) {
		started_ = false;
		delete data;
		//LOG_SYSFATAL << "Failed in pthread_create";
	}
}

int Thread::join() {
	assert(started_);
	assert(!joined_);
	joined_ = true;
	return pthread_join(pthreadId_, NULL);
}

} // namespace tiny
