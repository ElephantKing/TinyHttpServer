#ifndef TINY_POLLER_H
#define TINY_POLLER_H

#include <map>
#include <vector>

#include "../base/Timestamp.h"
#include "EventLoop.h"

namespace tiny {

class Channel;

class Poller {
public:
	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop* loop);
	Poller(const Poller&) = delete;
	virtual ~Poller();

	virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

	virtual void updateChannel(Channel* channel) = 0;

	virtual void removeChannel(Channel* channel) = 0;

	virtual bool hasChannel(Channel* channel) const;

	static Poller* newDefaultPoller(EventLoop *loop);

	void assertInLoopThread() const {
		ownerLoop_->assertInLoopThread();
	}

protected:
	typedef std::map<int, Channel*> ChannelMap;
	ChannelMap channels_;

private:
	EventLoop* ownerLoop_;

};
} //namespace tiny
#endif

