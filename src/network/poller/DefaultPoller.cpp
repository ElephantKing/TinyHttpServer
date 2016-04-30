#include "PollPoller.h"

namespace tiny {

Poller* Poller::newDefaultPoller(EventLoop *loop) {
	return new PollPoller(loop);
}

}//tiny

