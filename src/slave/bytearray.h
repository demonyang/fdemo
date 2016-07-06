//Author: demon1991yl@gmail.com

#ifndef FDEMO_SLAVE_BYTEARRAY_H_
#define FDEMO_SLAVE_BYTEARRAY_H_

#define mi_uint5korr(A) ((ulonglong)(((uint32) (((uchar*) (A))[4])) +\
                                    (((uint32) (((uchar*) (A))[3])) << 8) +\
                                    (((uint32) (((uchar*) (A))[2])) << 16) +\
                                    (((uint32) (((uchar*) (A))[1])) << 24)) +\
                                    (((ulonglong) (((uchar*) (A))[0])) << 32))


#define mi_sint2korr(A) ((int16)(((int16) (((uchar*) (A))[1])) +\
                                    (((int16) (((uchar*) (A))[0])) << 8)))

#define mi_sint3korr(A) ((int32) (((((uchar*) (A))[0]) & 128) ? \
            (((uint32) 255L << 24) | \
            (((uint32) ((uchar*) (A))[0]) << 16) |\
            (((uint32) ((uchar*) (A))[1]) << 8) | \
            ((uint32) ((uchar*) (A))[2])) : \
            (((uint32) ((uchar*) (A))[0]) << 16) |\
            (((uint32) ((uchar*) (A))[1]) << 8) | \
            ((uint32) ((uchar*) (A))[2])))

#define DATETIMEF_INT_OFS 0x8000000000LL
#include <string>
#include <string.h>
#include <stdexcept>

namespace fdemo{
namespace slave{

class MalformException: public std::length_error {
public:
	MalformException(const std::string &msg): std::length_error(msg) {}
};	//MalformException

class ByteArray{
public:
    ByteArray(): data_(NULL), size_(0), offset_(0) {} 
    ~ByteArray() {}

    void assign(const char* data, size_t size) {
        data_ = data;
        size_ = size;
        offset_ = 0;
    }

    size_t size() { return size_; }
    size_t position() { return offset_; }
    size_t reamin() { return size_-offset_; }

    uint8_t getFixed8() const;
    uint16_t getFixed16() const;
    uint16_t getFixed16_little() const;
    uint32_t getFixed24() const;
    uint32_t getFixed24_little() const;
    uint32_t getFixed32() const;
    uint64_t getFixed40_little() const;
    uint64_t getFixed48() const;
    uint64_t getFixed64() const;
    uint64_t getVarint() const;
    float getFloat() const;

    const char* get(size_t len) const;
    std::string getLeft() const;


private:
    const char * data_;
    size_t size_;
    mutable size_t offset_;

};

} //namespace slave
} //namespace fdemo

#endif

