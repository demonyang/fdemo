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
    ColumnMeta cm;
    ByteArray ba;
    int size;
    int len = bytes.getVarint(&size);
    const char* ptr = bytes.get(len);
    std::string tmpStr = std::string(ptr, len);
    ba.assign(tmpStr.data(), tmpStr.size());
    cm.unpack(ba, columntype);
    columnmeta = cm.meta;
}

void ColumnMeta::unpack(const ByteArray& bytes, std::vector<uint8_t> columntype){
    for(auto loop = columntype.begin();loop != columntype.end(); loop++) {
        switch (*loop) {
            case fieldTypeTinyBLOB:
            case fieldTypeBLOB:
            case fieldTypeMediumBLOB:
            case fieldTypeLongBLOB:
            case fieldTypeDouble:
            case fieldTypeFloat:
            case fieldTypeGeometry:
            case fieldTypeTimestamp2:
            case fieldTypeDatetime2:
            case fieldTypeTime2:
            {
                uint8_t t = bytes.getFixed8();
                meta.push_back(t);
                break;
            }
            //attention bigendian
            case fieldTypeSet:
            case fieldTypeString:
            case fieldTypeEnum:
            case fieldTypeNewDecimal:
            {
                const char* d = bytes.get(2);
                uint16_t i = (uint16_t) (((uint16_t) ((unsigned char) (d)[1])) + ((uint16_t) ((unsigned char) (d)[0]) << 8));
                meta.push_back(i);
                break;
            }
            case fieldTypeBit:
            case fieldTypeVarChar:
            {
                uint16_t t =bytes.getFixed16();
                meta.push_back(t);
                break;
            }
            default:
            {
            meta.push_back(0);
            break;
            }
        }
    }
}

void RowsEvent::unpack(const ByteArray& bytes) {
    tableid = bytes.getFixed48();
    flags = bytes.getFixed16();
    extra_data = bytes.get(bytes.getFixed16()-2);
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
