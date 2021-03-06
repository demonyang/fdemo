//Author: demon1991yl@gmail.com

#ifndef FDEMO_BINLOGPARSE_MOCK_SLAVE_H_
#define FDEMO_BINLOGPARSE_MOCK_SLAVE_H_

#include "metadata.h"
#include "glog/logging.h"
#include <mysql/mysql.h>
#include <mysql/my_global.h>
#include <mysql/m_ctype.h>
#include <mysql/sql_common.h>
#include <mysql/mysql_com.h>
#include "binlogparse/logevent.h"
#include <exception>
#include "binlogparse/Tableschema.h"
#include <map>

//struct st_mysql;
//struct st_mysql_res;

namespace fdemo{
namespace binlogparse{

enum RowValueType{
    RowBefore,
    RowAfter,
};

struct RowValue{
    LogEvent::Type rowType;
    std::string db;
    std::string table;
    std::vector<std::string> columns;
    std::vector<std::string> beforeValue;
    std::vector<std::string> afterValue;
};

class EventAction {
public:
    virtual ~EventAction() {}
    virtual int onRowsEvent(const RowsEvent& event, std::vector<RowValue>& rows) = 0;
    //virtual int onQueryEvent() = 0;
};

class MockSlave {

public:
    MockSlave(){}
    ~MockSlave(){ Close(); }

    int Connect(const std::string& host, int port, const std::string& user, const std::string& passwd);
    int DumpBinlog(uint32_t ServerId, const std::string& filename, uint32_t offset);
    int run(EventAction* eventaction);
    st_mysql_res* query(const char* sql, int len);
    void Close();

private:
    int readEvent(LogEvent* header, ByteArray* body);
    int processEvent(LogEvent header, ByteArray body, EventAction* eventaction);
    int onRotateEvent(const RotateEvent& event);
    int onTableMapEvent(const TableMapEvent& evnet);
    int onRowsEvent(const RowsEvent& event, EventAction* eventaction);
    int setCrc32();
    //parse one row data
    void unpackRow(RowValue *row, RowValueType rvt, const RowsEvent& event, const ByteArray& bytes, TableSchema *table);
    

private:
    st_mysql* slave_; //for read binlog
    st_mysql* schema_; //for query

    uint32_t ServerId_;
    std::string filename_;
    uint32_t offset_;

    std::map<uint64_t, TableSchema*> tables_;
    //struct BinlogInfo master_info_;
};

} // namespace binlogparse
} // namespace fdemo


#endif
