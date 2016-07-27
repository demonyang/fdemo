//Author: demon1991yl@gmail.com

#ifndef FDEMO_SLAVE_LOGEVENT_H
#define FDEMO_SLAVE_LOGEVENT_H

#include <cstdint>
#include <vector>
#include "slave/bytearray.h"

namespace fdemo{
namespace slave{

enum FieldType {
    fieldTypeDecimal,
    fieldTypeTiny,
    fieldTypeShort,
    fieldTypeLong,
    fieldTypeFloat,
    fieldTypeDouble,
    fieldTypeNULL,
    fieldTypeTimestamp,
    fieldTypeLongLong,
    fieldTypeInt24,
    fieldTypeDate,
    fieldTypeTime,
    fieldTypeDateTime,
    fieldTypeYear,
    fieldTypeNewDate,
    fieldTypeVarChar,
    fieldTypeBit,
    fieldTypeTimestamp2,
    fieldTypeDatetime2,
    fieldTypeTime2,
    fieldTypeNewDecimal = 0xf6,
    fieldTypeEnum = 0xf7,
    fieldTypeSet = 0xf8,
    fieldTypeTinyBLOB = 0xf9,
    fieldTypeMediumBLOB = 0xfa,
    fieldTypeLongBLOB = 0xfb,
    fieldTypeBLOB = 0xfc,
    fieldTypeVarString = 0xfd,
    fieldTypeString = 0xfe,
    fieldTypeGeometry = 0xff,
};

struct LogEvent{
    enum Type {
        UNKNOWN_EVENT = 0x00,
        START_EVENT_V3 = 0x01,
        ROTATE_EVENT = 0x04,
        TABLE_MAP_EVNET = 0x13,
        //mysql5.1.15 to 5.6.x
        WRITE_ROWS_EVENTv1 = 0x17,
        UPDATE_ROWS_EVENTv1 = 0x18,
        DELETE_ROWS_EVENTv1 = 0x19,
        //mysql 5.6.x
        WRITE_ROWS_EVENTv2 = 0x1e,
        UPDATE_ROWS_EVENTv2 = 0x1f,
        DELETE_ROWS_EVENTv2 = 0x20,
    };
    uint32_t timestamp;
    Type type;
    uint32_t server_id;
    uint32_t event_size;
    uint32_t logpos;
    uint16_t flags;

    virtual void unpack(const ByteArray& bytes);

    const char* getEventName() const {
        switch (type) {
            case ROTATE_EVENT:
                return "ROTATE_EVENT";
            case TABLE_MAP_EVNET:
                return "TABLE_MAP_EVNET";
            case WRITE_ROWS_EVENTv1:
                return "WRITE_ROWS_EVENTv1";
            case UPDATE_ROWS_EVENTv1:
                return "WRITE_ROWS_EVENTv1";
            case DELETE_ROWS_EVENTv1:
                return "WRITE_ROWS_EVENTv1";
            case WRITE_ROWS_EVENTv2:
                return "WRITE_ROWS_EVENTv2";
            case UPDATE_ROWS_EVENTv2:
                return "WRITE_ROWS_EVENTv2";
            case DELETE_ROWS_EVENTv2:
                return "WRITE_ROWS_EVENTv2";
            default:
                return "UNKNOWN_LOG_EVENT";
        }
    }

};

//defination of bitmap
class BitMap{
public:
    BitMap(): len_(0) {}
    ~BitMap(){}

    void unpack(int len, const ByteArray& bytes);
    bool isSet(int pos);
private:
    int len_;
    std::string bits_;
};

struct RotateEvent: public LogEvent {
    uint64_t position; //binlog-version > 1
    std::string filename;

    RotateEvent(const LogEvent& header) :LogEvent(header){}
    virtual void unpack(const ByteArray& bytes);
};

struct TableMapEvent : public LogEvent{
    uint64_t tableid;
    uint16_t flags;
    std::string dbname;
    std::string tablename;
    uint64_t columncount;
    std::vector<uint8_t> columntype;
    std::vector<uint16_t> columnmeta;

    TableMapEvent(const LogEvent& header) :LogEvent(header){}
    virtual void unpack(const ByteArray& bytes);
};

struct RowsEvent: public LogEvent{
    uint64_t tableid;
    uint16_t flags;
    //version = 2
    std::string extra_data;
    uint64_t columncount;
    BitMap beforeBM;
    BitMap afterBM;
    std::string values;

    RowsEvent(const LogEvent& header) :LogEvent(header) {}
    virtual void unpack(const ByteArray& bytes);
};

struct ColumnMeta{
    std::vector<uint16_t> meta;
    void unpack(const ByteArray& bytes, std::vector<uint8_t> columntype);
};

} // namespace slave
} // namespace fdemo
#endif
