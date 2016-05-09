#ifndef _TINY_TIMESTAMP_H_
#define _TINY_TIMESTAMP_H_

#include <cstdlib>
#include "StringPiece.h"

namespace tiny {

class Timestamp {

public:
	Timestamp()
		: microSecondsSinceEpoch_(0)
	{ }

	explicit Timestamp(int64_t microSecondsSinceEpochArg) 
		: microSecondsSinceEpoch_(microSecondsSinceEpochArg)
	{ }

	string toString() const;
	string toFormattedString(bool showMicroseconds = true) const;

	bool valid() const { return microSecondsSinceEpoch_ > 0; }

	int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

	time_t secondsSinceEpoch() const {
		return static_cast<time_t>(microSecondsSinceEpoch_ / kMircoSecondsPerSecond);
	}

	static Timestamp now();
	
	static Timestamp invalid() {
		return Timestamp();
	}

	static Timestamp fromUinxTime (time_t t, int microseconds) {
		return Timestamp(static_cast<int64_t>(t) * kMircoSecondsPerSecond + microseconds);
	}

	static const int kMircoSecondsPerSecond = 1000 * 1000;

private:
	int64_t microSecondsSinceEpoch_;
};

inline bool operator==(Timestamp lhs, Timestamp rhs) {
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp high, Timestamp low) {
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::kMircoSecondsPerSecond;
}

}//namespace tiny

#endif

