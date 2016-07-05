g++-5 -I../network -I../base -I/usr/local/Cellar/boost/1.60.0_1/include \
	../base/Thread.cpp \
	../base/Exception.cpp \
	../base/ThreadPool.cpp \
	../network/EventLoop.cpp \
	../network/Channel.cpp \
	../network/SocketsOps.cpp \
	../network/Poller.cpp \
	../network/poller/DefaultPoller.cpp \
	../network/poller/PollPoller.cpp \
	../base/Timestamp.cpp \
	EventLoopTest.cpp \
	-g -std=c++11 -o test;
#./test

#	../network/Socket.cpp\
#	../network/Channel.cpp \

#ThreadAndCondition.cpp \
#ThreadAndSync.cpp
#ThreadPoolAndSync.cpp \
#ThreadPoolAndCondition.cpp \
