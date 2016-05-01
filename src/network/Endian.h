#ifndef TINY_ENDIAN_H
#define TINY_ENDIAN_H

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <type_traits>

namespace {

//按字节反转
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type 
reverse_bytes(T t) {
	void *startAddr = &t;
	std::reverse((uint8_t*)startAddr, (uint8_t*)startAddr + sizeof(T));
	return t;
}

//既不是大端也不是小端，直接退出
template<bool isBigEndian_, bool isLittleEndian_, typename T>
struct EndianHelper {
	static typename std::enable_if<std::is_integral<T>::value, T>::type
	trans(T) {
		abort();
	}
};

//转换成网络序
template<typename T>
struct EndianHelper<false, true, T> {
	static typename std::enable_if<std::is_integral<T>::value, T>::type
	trans(T t) {
		return reverse_bytes(t);
	}
};

template<typename T>
struct EndianHelper<true, false, T> {
	static typename std::enable_if<std::is_integral<T>::value, T>::type
	trans(T t) {
		return t;
	}
};


#if __BYTE_ORDER == __BIG_ENDIAN
typedef std::true_type  isBigEndian;
typedef std::false_type isLittleEndian;
#elif  __BYTE_ORDER == __LITTLE_ENDIAN
typedef std::false_type isBigEndian;
typedef std::true_type isLittleEndian;
#else
#error "Endian not defined"
#endif


}

namespace tiny {

namespace sockets {

template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
hostToNetwork(T t) {
	return EndianHelper<isBigEndian::value, isLittleEndian::value, T>::trans(t);
}


template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
networkToHost(T t) {
	return EndianHelper<isLittleEndian::value, isBigEndian::value, T>::trans(t);
}

}// namespace sockets


}// namespace tiny
#endif

