//Author: demon1991@gmail.com

#include "slave/logevent.h"

namespace fdemo{
namespace slave{

void LogEvent::unpack(const ByteArray& bytes) {
    timestamp = bytes.getFixed32();
    type = (Type)bytes.getFixed8();
    server_id = bytes.getFixed32();
    event_size = bytes.getFixed32();
    logpos = bytes.getFixed32();
    flags = bytes.getFixed16();
}

void BitMap::unpack(int len, const ByteArray& bytes) {
    len_ = len;
    size_t tmplen = (len+7)/8;
    bits_.assign(bytes.get(tmplen), tmplen);
}

bool BitMap::isSet(int pos) {
    if(pos < 0 || pos >= len_) {
        return false;
    }
    return (bits_[pos/8] >> (pos%8)) & 0x01;
}

void RotateEvent::unpack(const ByteArray& bytes) {
    position = bytes.getFixed64();   
    filename = bytes.getLeft();
}

void TableMapEvent::unpack(const ByteArray& bytes) {
    tableid = bytes.getFixed48();
    flags =  bytes.getFixed16();
    dbname = bytes.get(bytes.getFixed8() +1);
    tablename = bytes.get(bytes.getFixed8() +1);
    columncount = bytes.getVarint();
    for(uint64_t i = 0;i<columncount;i++) {
        uint8_t t = bytes.getFixed8();
        columntype.push_back(t);
    }
}

void RowsEvent::unpack(const ByteArray& bytes) {
    tableid = bytes.getFixed48();
    flags = bytes.getFixed16();
    extra_data = bytes.get(bytes.getFixed16());
    columncount = bytes.getVarint();
    if(type == LogEvent::UPDATE_ROWS_EVENTv2 || type ==LogEvent::DELETE_ROWS_EVENTv2) {
        beforeBM.unpack(columncount, bytes);
    }
    if(type == LogEvent::UPDATE_ROWS_EVENTv2 || type == LogEvent::WRITE_ROWS_EVENTv2) {
        afterBM.unpack(columncount, bytes);
    } 
    values = bytes.getLeft();
}

} // namespace slave
} //namespace fdemo
