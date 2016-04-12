#ifndef _TINY_POLLER_H_
#define _TINY_POLLER_H_

#include <map>
#include <vector>

#include "../base/Timestamp.h"
#include "EvenLoop.h"

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

	virtual bool hashChannel(Channel* channel) const;

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

