#include "EventLoop.h"

#include "../base/Mutex.h"
#include "Channel.h"
#include "Poller.h"

#include <functional>
#include <signal.h>
//#include <sys/event.h>

namespace {

__thread tiny::EventLoop* t_loopInThisThread = 0;

const int kPoolTimeMs = 10000;

int createEventfd() {
//	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
//	if (evtfd < 0) {
//		//LOG_SYSERR << "Failed in eventfd";
//		abort();
//	}
//	return evtfd;
	return 0;
}

class IgnoreSigPipe {
public:
	IgnoreSigPipe() {
		::signal(SIGPIPE, SIG_IGN);
	}
};

IgnoreSigPipe iniObj;
} //unanmed namespace

namespace tiny {

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
	return t_loopInThisThread;
}

EventLoop::EventLoop()
	: looping_(false),
	  quit_(false),
	  eventHandling_(false),
	  callingPendingFunctors_(false),
	  iteration_(0),
	  threadId_(CurrentThread::tid()),
//	  poller_(Poller::newDefaultPoller(this)),
	  TimerQueue(new TimerQueue(this)),
	  wakeupFd_(createEventfd()),
	  wakeupFd_(createEventfd()),
	  wakeChannel_(new Channel(this, wakeupFd_)),
	  currentActiveChannel_(NULL) 
{
	//LOG_DEBUG << "EventLoop created " << this << "in thread " << theradId_; 		  
	if (t_loopInThisThread) {
//		LOG_FATAL << "Another EventLoop " << t_loopInThisThread
//			      << " exists in this thread " << threadId_;
	} else {
		t_loopInThisThread = this;
	}
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}
}

