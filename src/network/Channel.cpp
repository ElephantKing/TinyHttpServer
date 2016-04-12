#include "Channel.h"
#include "EvenLoop.h"

#include <sstream>
#include <poll.h>


namespace tiny {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd__)
	: loop_(loop), 
	  fd_(fd__),
	  events_(0),
	  revents_(0),
	  index_(-1),
	  logHup_(true),
	  tied_(false), 
	  eventHandling_(false), 
	  addedToLoop_(false) 
{
}

Channel::~Channel() {
	assert(!eventHandling_);
	assert(!addedToLoop_);
	if (loop_->isInLoopThread()) {
		assert(!loop_->hasChannel(this));
	}
}

void Channel::tie(const std::shared_ptr<void>& obj) {
	tie_ = obj;
	tied_ = true;
}

void Channel::update() {
	addedToLoop_ = true;
	loop_->updateChannel(this);
}

void Channel::remove() {
	assert(isNoneEvent());
	addedToLoop_ = false;
	loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime) {
	std::shared_ptr<void> guard;
	if (tied_) {
		guard = tie_.lock();
		if (guard) {
			handleEventWithGuard(receiveTime);
		}
	}
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
	eventHandling_ = true;
	//LOG_TRACE << reventsToString();
	if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
		if (logHup_) {
			//LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLHUP";
		}
		if (closeCallback_) closeCallback_();
	}

	if (revents_ & POLLNVAL) {
		//LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLNVAL";
	}

	if (revents_ & (POLLERR | POLLNVAL)) {
		if (errorCallback_) errorCallback_();
	}

	if (revents_ & (POLLIN | POLLPRI /* | POLLRDHUP*/)) {
		if (readCallback_) readCallback_(receiveTime);
	}

	if (revents_ & POLLOUT) {
		if (writeCallback_) writeCallback_();
	}
	eventHandling_ = false;
}

string Channel::reventsToString() const {
	return eventsToString(fd_, revents_);
}

string Channel::eventsToString(int fd, int event) {
	std::ostringstream oss;
	oss << fd << " : ";
	if (event & POLLIN) {
		oss << "IN ";
	}

	if (event & POLLPRI) {
		oss << "PRI ";
	}

	if (event & POLLOUT) {
		oss << "OUT ";
	}

	if (event & POLLHUP) {
		oss << "HUP";
	}

	//if (event & POLLRDHUP) {
	//	oss << "RDHUP";
	//}

	if (event & POLLERR) {
		oss << "ERR ";
	}

	if (event & POLLNVAL) {
		oss << "NVAL ";
	}
	return oss.str().c_str();
}

}//namespace tiny
