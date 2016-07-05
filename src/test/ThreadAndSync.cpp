#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"

#include <iostream>
#include <vector>
#include <memory>

using namespace tiny;
using namespace std;

class NumCounts {
public:
	NumCounts() : cnt(0) { }
	void count_and_say() {
		MutexLockGuard lock(mutex_);
		cout << "CurrentThread:" << CurrentThread::tid() << " ";
		cout << "Count and Say: " << cnt++ << endl;
	}
	NumCounts(const NumCounts&) = delete;
private:
	mutable MutexLock mutex_;
	size_t cnt;
};


int main() {
	NumCounts nc;
	vector<unique_ptr<Thread>> thread_vec;
	for (int i = 0; i < 10; ++i) {
		thread_vec.emplace_back(unique_ptr<Thread>(new
			Thread(std::bind([&]() { 
				for (size_t i = 0; i < 100;  ++i) {
				nc.count_and_say(); 
			}
		}))));
	}
	
	cout << "MainThread: " << CurrentThread::tid() << endl;
	for (auto& thread : thread_vec) {
		thread->start();
	}
	for (auto& thread : thread_vec) {
		thread->join();
	}
	return 0;
}

