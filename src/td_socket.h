#include <sys/socket.h>
#include "td_IPAddress.h"


namespace td {

class socket {
public:
	socket();
	socket(int descriptor);
	socket(IPAddress);
	int setBlock();
	virtual int readn(char *, size_t);
	virtual int writen(char*, size_t);
	int shutdown();

protected:
	bool _block;
};

}

