#include <string>

namespace td {

typedef int32_t ipv4_t;
class IPAddr {
public:
	explicit IPAddr(const std::string&);
	explicit IPAddr(ipv4_t);
	std::string toString();
	ipv4_t IPAddress();
private:
	ipv4_t _ip;
	char getByteFromString(const std::string&, unsigned int&, bool&);
};

}
