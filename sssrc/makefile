objects = main.o env.o config.o myhttp.o logHelper.o  httpReq.o
flag = -std=c++11 -Wall -g

server : $(objects)
	g++-5 -o server $(flag) $(objects) 

main.o : env.h config.h myhttp.h logHelper.h \
	main.cpp
	g++-5 -c $(flag) main.cpp

env.o : env.h \
	env.cpp
	g++-5 -c $(flag) env.cpp

config.o : env.h config.h \
	config.cpp
	g++-5 -c $(flag) config.cpp

myhttp.o : env.h myhttp.h logHelper.h\
	myhttp.cpp
	g++-5 -c $(flag) myhttp.cpp

logHelper.o : logHelper.h\
	logHelper.cpp
	g++-5 -c $(flag) logHelper.cpp

httpReq.o : httpReq.h \
	httpReq.cpp
	g++-5 -c $(flag) httpReq.cpp
