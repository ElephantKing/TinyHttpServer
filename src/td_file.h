#include "td_IODevice.h"
#include <string>

namespace td {

extern const size_t default_io_buff_size;
class file : public IODevice {
public:
	file(size_t buffsize = default_io_buff_size) : buff(new char[buffsize]) {}
	int descriptor() { return _descriptor; }
	virtual int readn(char *, size_t);
	virtual int writen(char *, size_t);
	int open(const std::string&);
	int close();

protected:
	int _descriptor;
	char* buff;
};

}
