#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"
#include "ThreadPool.h"

#include <iostream>
#include <vector>
#include <queue>

using namespace tiny;
using namespace std;

class BlockQueue {
public:
	BlockQueue(int size = -1) :
		mutex_(),
		notEmpty(mutex_),
		notFull(mutex_),
		size_(size)
	{ }

	int take() {
		MutexLockGuard lock(mutex_);
		while (queue_.size() == 0) {
			notEmpty.wait();
		}
		int item = queue_.front();
		queue_.pop();
		cout << "Thread[" << CurrentThread::tid() << "] take " << item << ", rest: " << queue_.size() << endl; 
		if (size_ != -1) {
			notFull.notify();
		}
		return item;
	}
	
	void push(int item) {
		MutexLockGuard lock(mutex_);
		assert(size_ > -2);
		while (size_ > 0 && queue_.size() == static_cast<size_t>(size_)) {
			notFull.wait();
		}
		queue_.push(item);
		cout << "Thread[" << CurrentThread::tid() << "] push " << item << ", rest: " << queue_.size() << endl;
		notEmpty.notify();
	}
	
	size_t size() {
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}
private:
	MutexLock mutex_;
	Condition notEmpty;
	Condition notFull;
	queue<int> queue_;
	int size_;
};

int main() {
	BlockQueue bq;
	std::function<void()> push = [&] () {
		for (int i = 0; i < 20; ++i) {
			bq.push(1);
		}
	};
	std::function<void()> take = [&] () {
		for (int i = 0; i < 20; ++i) {
			bq.take();
		}
	};
	ThreadPool tp1("push");
	ThreadPool tp2("take");
	tp1.setMaxQueueSize(100);
	tp2.setMaxQueueSize(100);
	tp1.start(10);
	tp2.start(10);
	for (int i = 0; i < 20; ++i) {
		tp1.run(push);
	}
	for (int i = 0; i < 20; ++i) {
		tp2.run(take);
	}
	while (1)
		;
	return 0;
}
