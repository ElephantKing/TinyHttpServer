#ifndef _TINY_POLLPOLLER_H_
#define _TINY_POLLPOLLER_H_

#include "../Poller.h"

#include <vector>

struct pollfd;

namespace tiny {


class PollPoller : public Poller {
public:
	PollPoller(EventLoop *loop);
	virtual ~PollPoller();

	virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
	virtual void updateChannel(Channel* channel);	
	virtual void removeChannel(Channel* channel);	

private:
	void fillActiveChannels(int numEvents,
			ChannelList *activeChannels) const;

	typedef std::vector<struct pollfd> PollFdList;
	PollFdList pollfds_;
};                                               
                                                 	

}//namespace tiny;
#endif

