#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"

#include <iostream>
#include <queue>
#include <vector>

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
	mutable MutexLock mutex_;
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
	vector<unique_ptr<Thread>> thread_vec;
	for (int i = 0; i < 5; ++i) {
		thread_vec.emplace_back(unique_ptr<Thread>(new Thread(push)));
	}
	for (int i = 0; i < 5; ++i) {
		thread_vec.emplace_back(unique_ptr<Thread>(new Thread(take)));
	}
	for (auto& thread : thread_vec) {
		thread->start();
	}
	for (auto& thread : thread_vec) {
		thread->join();
	}
	return 0;
}
