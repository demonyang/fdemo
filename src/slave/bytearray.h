//Author: demon1991yl@gmail.com

#ifndef FDEMO_SLAVE_BYTEARRAY_H_
#define FDEMO_SLAVE_BYTEARRAY_H_

#include<string>

namespace fdemo{
namespace slave{

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
    uint32_t getFixed24() const;
    uint32_t getFixed32() const;
    uint64_t getFixed48() const;
    uint64_t getFixed64() const;
    uint64_t getVarint() const;

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

