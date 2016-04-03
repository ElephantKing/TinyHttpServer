#include "td_ipaddr.h"
#include <cstdio>
#include <iostream>

using namespace std;
using namespace td;

td::IPAddr::IPAddr(ipv4_t addr) : _ip(addr) {}

td::IPAddr::IPAddr(const string& addr) {
	char *p = reinterpret_cast<char *>(&_ip);
	unsigned int index = 0;
	for (int i = 0; i < 4; ++i) {
		int byte = getByteFromString(addr, index);
		*(p + i) = byte;
		//cout << hex << _ip << endl;
	}
}


char td::IPAddr::getByteFromString(const string& addr, unsigned int& index, bool &result) {
	int byte = 0;
	while(index < addr.size() && addr[index] != '.') {
		if (addr[index] >= '0' && addr[index] <= '9') {
			byte *= 10;
			byte += addr[index++] - '0';
		} else {
			return 0;
		}
	}
	if (index < addr.size() && addr[index] == '.') {
		++index;
	}
	if (byte < 256 && byte >= 0) {
		return byte;
	}
	return 0;
}

