#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"
#include "ThreadPool.h"

#include <iostream>
#include <vector>

using namespace tiny;
using namespace std;

class NumCounts {
public:
	NumCounts() : cnt(0) { }
	void count_and_say() {
		MutexLockGuard lock(mutex_);
		cout << "Count and Say: " << cnt++ << endl;
		cout << "CurrentThread:" << CurrentThread::tid() << " ";
	}
private:
	MutexLock mutex_;
	size_t cnt;
};


int main() {
	NumCounts nc;
	std::function<void()> f = std::bind([&]() { 
		for (size_t i = 0; i < 100;  ++i) {
		nc.count_and_say(); 
		}});
	ThreadPool tp;
	tp.setMaxQueueSize(100);
	tp.start(10);
	for (int i = 0; i < 20; ++i) {
		tp.run(f);
	}
	return 0;
}
