#ifndef _TINY_CHANNEL_H_
#define _TINY_CHANNEL_H_

#include <functional>
#include <string>
#include <memory>

#include "../base/Timestamp.h"

namespace tiny {

using std::string;
class EventLoop;

class Channel {
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(Timestamp)> ReadEventCallback;

	Channel(EventLoop* loop, int fd);
	Channel(const Channel&) = delete;
	~Channel();

	void handleEvent(Timestamp receiveTime);

	void setReadCallback(const ReadEventCallback& callback)
	{ readCallback_ = callback; }
	void setWriteCallback(const EventCallback& callback) 
	{ writeCallback_ = callback; }
	void setCloseCallback(const EventCallback& callback)
	{ closeCallback_ = callback; }
	void setErrorCallback(const EventCallback& callback)
	{ errorCallback_ = callback; }

	void setReadCallback(ReadEventCallback&& callback)
	{ readCallback_ = std::move(callback); }
	void setWriteCallback(EventCallback&& callback) 
	{ writeCallback_ = std::move(callback); }
	void setCloseCallback(EventCallback&& callback)
	{ closeCallback_ = std::move(callback); }
	void setErrorCallback(EventCallback&& callback)
	{ errorCallback_ = std::move(callback); }

	void tie(const std::shared_ptr<void>&);

	int fd() { return fd_; }
	int events() const { return events_; }
	void set_revents(int revt) { revents_ = revt; }
	bool isNoneEvent() const { return events_ == kNoneEvent; }

	void enableReading() { events_ |= kReadEvent; update(); }
	void disableReading() { events_ &= ~kReadEvent; update(); }
	void enableWriting() { events_ |= kWriteEvent; update(); }
	void disableWriting() { events_ &= ~kWriteEvent; update(); }
	void disableAll() { events_ = kNoneEvent; update(); }

	bool isWriting() const { return events_ & kWriteEvent; }
	bool isReading() const { return events_ & kReadEvent; }

	int index() { return index_; }
	void set_index(int idx) { index_ = idx; }

	string reventsToString() const;
	string eventsToString() const;

	void doNotLogHup() { logHup_ = false; }

	EventLoop* ownerLoop() { return loop_; }
	void remove();

private:
	static string eventsToString(int fd, int event);

	void update();
	void handleEventWithGuard(Timestamp receiveTime);

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	EventLoop* loop_;
	const int fd_;
	int events_;
	int revents_;
	int index_;
	bool logHup_;

	std::weak_ptr<void> tie_;
	bool tied_;
	bool eventHandling_;
	bool addedToLoop_;
	ReadEventCallback readCallback_;
	EventCallback writeCallback_;
	EventCallback closeCallback_;
	EventCallback errorCallback_;
};

}// namespace tiny
#endif

