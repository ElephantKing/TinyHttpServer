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
	../network/EventLoopThread.cpp\
	../network/Socket.cpp\
	../base/Timestamp.cpp \
	../network/Acceptor.cpp\
	../network/InetAddress.cpp\
	../network/Endian.cpp\
	AcceptorAndEventLoopThreadPool.cpp\
	-g -std=c++11 -o test;
#./test


#ChannelAndPollerTest.cpp \
#ThreadPoolAndSync.cpp \
#EventLoopTest.cpp \
#ThreadAndCondition.cpp \
#ThreadAndSync.cpp
#ThreadPoolAndCondition.cpp \

