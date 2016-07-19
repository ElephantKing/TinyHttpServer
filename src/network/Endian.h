#ifndef TINY_ENDIAN_H
#define TINY_ENDIAN_H

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <type_traits>
#include <iostream>

namespace {

template<typename T>
T reverseOrder(T t) {
#if BYTE_ORDER == BIG_ENDIAN
	return t;

#elif BYTE_ORDER == LITTLE_ENDIAN
	unsigned char * byte_ptr = reinterpret_cast<unsigned char *>(&t);
	std::reverse(byte_ptr, byte_ptr + sizeof(t));
	return t;
#else
#error "byte order error, in Endian.h"
#endif
}

}

namespace tiny {

namespace sockets {

int16_t hostToNetwork16(int16_t val);

int32_t hostToNetwork32(int32_t val);

int64_t hostToNetwork64(int64_t val);

int16_t networkToHost16(int16_t val);

int32_t networkToHost32(int32_t val);

int64_t networkToHost64(int64_t val);

}// namespace sockets

}// namespace tiny
#endif

