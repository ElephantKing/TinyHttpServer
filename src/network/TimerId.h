#ifndef _TINY_TIMERID_H_
#define _TINY_TIMERID_H_

#include <cstdlib>

namespace tiny {

class Timer;
class TimerId {
public:
	TimerId()
		: timer_(NULL),
		  sequence_(0)
	{
	}

	TimerId(Timer* timer, int64_t seq)
		: timer_(timer),
		  sequence_(seq)
	{
	}

	friend class TimerQueue;

private:
	Timer *timer_;
	int64_t sequence_;
};
}// tiny
#endif

