#ifndef TINY_TIMERQUEUE_H
#define TINY_TIMERQUEUE_H

namespace tiny {

class EventLoop;
class Timer;
class TimerId;

class TimerQueue {

public:
	TimerQueue(EventLoop *loop);
	~TimerQueue();
	TimerQueue(const TimerQueue&) = delete;
};

} // namespace tiny
#endif

