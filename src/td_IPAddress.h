/*
 * 只支持IPv4 
 * 只支持IPv4 
 * 只支持IPv4 
 *
 * */
#include <string>

namespace td {

typedef int32_t ipv4_t;
class IPAddress {
public:
	explicit IPAddress(const std::string&);
	explicit IPAddress(ipv4_t);
	std::string toString();
	ipv4_t ipv4();
private:
	ipv4_t _ipv4;
	char getByteFromString(const std::string&, unsigned int&, bool&);
};

}
