#include <ctype.h>

namespace td {

class IODevice {
	virtual int readn(char *, size_t) = 0;
	virtual int writen(char *, size_t) = 0;
	virtual int close() = 0;
};

}

