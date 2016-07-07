
#include "slave/bytearray.h"
#include <mysql/my_global.h>

namespace fdemo{
namespace slave{

float ByteArray::getFloat() const {
	float nr;
	const char *ptr = get(4);

	float4get(nr, ptr);
	return nr;
}

uint64_t ByteArray::getVarint() const {
	const char *data = get(1);
	uint8_t i8 = (uint8_t) *data;
	if (i8 <= 250) {
		return (uint64_t)i8;
	}

	switch (i8) {
		case 251:
			return 0;
		case 252:
			return (uint64_t) getFixed16();
		case 253:
			return (uint64_t) getFixed32();
		case 254:
			return getFixed64();
		default:
			throw MalformException("invalid varint");
	}
}

uint64_t ByteArray::getFixed64() const {
	const char *d = get(8);
	uint64_t i = uint8korr(d);
	return i;
}

uint64_t ByteArray::getFixed48() const {
	const char *d = get(6);
	uint64_t i = uint5korr(d);
	return i;
}

uint32_t ByteArray::getFixed32() const {
	const char *d = get(4);
	uint32_t i = uint4korr(d);
	return i;
}

uint32_t ByteArray::getFixed24() const {
	const char *d = get(3);
	uint32_t i = uint3korr(d);
	return i;
}

uint16_t ByteArray::getFixed16() const {
	const char *d = get(2);
	uint16_t i = uint2korr(d);
	return i;
}

uint8_t ByteArray::getFixed8() const {
	const char *d = get(1);
	uint8_t i = (uint8_t) *d;
	return i;
}

const char *ByteArray::get(size_t len) const {
	if (size_ < len + offset_) {
		throw MalformException("byte array overflow");
	}
	const char *ptr = data_ + offset_;
	offset_ += len;
	return ptr;
}

std::string ByteArray::getLeft() const {
	const char *ptr = data_ + offset_;
	int len = size_ - offset_;
	offset_ = size_;
	return std::string(ptr, len);
}

} //namespace slave
} //namespace fdemo
