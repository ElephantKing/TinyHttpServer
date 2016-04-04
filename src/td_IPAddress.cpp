#include "td_IPAddress.h"
#include <cstdio>
#include <iostream>

using namespace std;
using namespace td;

td::IPAddress::IPAddress(ipv4_t addr) : _ipv4(addr) {}

td::IPAddress::IPAddress(const string& addr) {
	char *p = reinterpret_cast<char *>(&_ipv4);
	unsigned int index = 0;
	for (int i = 0; i < 4; ++i) {
		bool result = false;
		int byte = getByteFromString(addr, index, result);
		if (!result) {
			_ipv4 = 0;
			return;
		} else {
			*(p + i) = byte;
		}
	}
}


char td::IPAddress::getByteFromString(const string& addr, unsigned int& index, bool &result) {
	int byte = 0;
	while(index < addr.size() && addr[index] != '.') {
		if (addr[index] >= '0' && addr[index] <= '9') {
			byte *= 10;
			byte += addr[index++] - '0';
		} else {
			result = false;
			return 0;
		}
	}
	if (index < addr.size() && addr[index] == '.') {
		++index;
	}
	if (byte < 256 && byte >= 0) {
		result = true;
		return byte;
	}
	result = false;
	return 0;
}

