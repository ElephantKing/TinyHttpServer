#include "EventLoop.h"

#include "../base/Mutex.h"
#include "Channel.h"
#include "Poller.h"
#include "SocketsOps.h"

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
	  poller_(Poller::newDefaultPoller(this)),
	  TimerQueue(new TimerQueue(this)),
	  wakeupFd_(createEventfd()),
	  wakeupChannel_(new Channel(this, wakeupFd_)),
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

EventLoop::~EventLoop() {
	//LOG_DEBUG << "EventLoop " << this << "of thread " << threadId_
	//          << " destructs in thread " << CurrentThread::tid();
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
	t_loopInThisThread = NULL;
}

void EventLoop::loop() {
	assert(!looping_);
	assertInLoopThread();
	looping_ = true;
	quit_ = false; //FIXME : what if someone call quit() before loop() ?
//	LOG_TRACE << "EventLoop " << this << " start looping_";
	
	while(!quit_) {
		activeChannels_.clear();
		poolReturnTime_ = poller_->poll(kPoolTimeMs, &activeChannels_);
		++iteration_;
	//	if (Logger::logLevel() <= Logger::TRACE()) {
	//		printActiveChannels();
	//	}
		for (ChannelList::iterator it = activeChannels_.begin();
				it != activeChannels_.end(); ++it) {
			currentActiveChannel_ = *it;
			currentActiveChannel_->handleEvent(poolReturnTime_);
		}
		currentActiveChannel_ = NULL;
		eventHandling_ = false;
		doPendingFunctors();
	}

	//LOG_TRACE << "EventLoop " << this << " stop looping";
	looping_ = false;
}

void EventLoop::runInLoop(const Functor& callback) {
	if (isInLoopThread()) {
		callback();
	} else { 
		queueInLoop(callback);
	}
}

void EventLoop::queueInLoop(const Functor& callback) {
	{
	MutexLockGuard lock(mutex_);
	doPendingFunctors_.push_back(callback);
	}

	if (!isInLoopThread() || callingPendingFunctors_) {
		wakeup();
	}
}

TimerId EventLoop::runAt(const Timestamp &, const TimerCallback& ) {
//	return timerQueue_->addTimer(callback, time, 0.0);
	return TimerId();
}

TimerId EventLoop::runAfter(double, const TimerCallback&) {
	return TimerId();
}

TimerId EventLoop::runEvery(double, const TimerCallback&) {
	return TimerId();
}

void EventLoop::runInLoop(Functor&& callback) {
	if (isInLoopThread()) {
		callback();
	} else {
		queueInLoop(std::move(callback));
	}

}

void EventLoop::queueInLoop(Functor&& callback) {
	{
	MutexLockGuard lock(mutex_);
	doPendingFunctors_.push_back(std::move(callback));
	}

	if (!isInLoopThread() || callingPendingFunctors_) {
		wakeup();
	}
}

void EventLoop::cancel(TimerId) {
//	return timerQueue_->cancel(tierId);
}

void EventLoop::updateChannel(Channel* channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	if (eventHandling_) {
		assert(currentActiveChannel_ == channel ||
				std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
	}
	poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	return poller_->hasChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
	//LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
	//          << " was created in threadId_ = " << threadId_
	//          << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::wakeup() {
	uint64_t one = 1;	
	ssize_t n = sockets::write(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one)) {
//		LOG_ERROR << "EventLoop::wakeup() writes " << n << "bytes instead of 8";
	}
}

void EventLoop::handleRead() {
	uint64_t one = 1;
	ssize_t n = sockets::read(wakeupFd_, &one, sizeof(one));
	if (n != sizeof (one)) {
//		LOG_ERROR << "EventLoop::handleRead () reads " << n << " bytes instead of 8";
	}
}

void EventLoop::doPendingFunctors() {
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	{
	MutexLockGuard lock(mutex_);
	functors.swap(doPendingFunctors_);
	}

	for (size_t i = 0; i < functors.size(); ++i) {
		functors[i]();
	}
	callingPendingFunctors_ = false;
}

void EventLoop::printActiveChannels() const {
	for (ChannelList::const_iterator it = activeChannels_.begin();
		 it != activeChannels_.end(); ++it) 
	{
		const Channel* ch = *it; (void)ch;
//		LOG_TRACE << "{" << ch->reventsToString() << "}";
	}
}

}// tiny

