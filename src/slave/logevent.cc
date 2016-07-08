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

} // namespace slave
} //namespace fdemo
