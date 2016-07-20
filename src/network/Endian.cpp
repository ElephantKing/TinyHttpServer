#include "Endian.h"


namespace tiny {

namespace sockets {

int16_t hostToNetwork16(int16_t val) {
	return reverseOrder(val);
}

int32_t hostToNetwork32(int32_t val) {
	return reverseOrder(val);
}

int64_t hostToNetwork64(int64_t val) {
	return reverseOrder(val);
}

int16_t networkToHost16(int16_t val) {
	return reverseOrder(val);
}

int32_t networkToHost32(int32_t val) {
	return reverseOrder(val);
}

int64_t networkToHost64(int64_t val) {
	return reverseOrder(val);
}

}// namespace sockets

}// namespace tiny
