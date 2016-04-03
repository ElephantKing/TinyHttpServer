objects = td_socket.o td_ipaddr.o main.o
flags = -std=c++11 -Wall -g 

test : $(objects)
	clang++ -o test $(objects) $(flags)

td_socket.o : td_ipaddr.h td_socket.h \
	td_socket.cpp
	clang++ -c td_socket.cpp $(flags)

td_ipaddr.o : td_ipaddr.h \
	td_ipaddr.cpp
	clang++ -c td_ipaddr.cpp $(flags)

main.o : td_ipaddr.h td_socket.h \
	main.cpp
	clang++ -c main.cpp $(flags)

.PHONY : clean
clean :
	-rm test *.o
	-rm -rf test.*
	-rm *.h.gch
