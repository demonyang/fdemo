
#include "binlogevent/binlogsync.h"

namespace fdemo{
namespace binlogevent{

void BinlogSync::run() {
    int rc = 0;
    fdemo::slave::MockSlave reader;
    rc = reader.Connect(master_info_.host, master_info_.port, master_info_.user, master_info_.passwd);
    if(rc != 0) {
        LOG(ERROR)<<"Connect host:"<<master_info_.host<<", port:"<<master_info_.port<<"failed";    
        return;
    }

    rc = reader.DumpBinlog(master_info_.server_id, master_info_.default_file, master_info_.default_offset);
    if(rc != 0) {
        LOG(ERROR)<<"DumpBinlog failed,file:"<<master_info_.default_file<<", offset:"<<master_info_.default_offset;
    }
    rc = reader.run(this);
    if(rc != 0) {
        LOG(ERROR)<<"MockSlave::run failed";
    }
    LOG(INFO)<<"BinlogSync::run end!";
}

//consider parallel replication
//TODO
int BinlogSync::onRowsEvent(const fdemo::slave::RowsEvent& event, std::vector<fdemo::slave::RowValue> rows) {
    int rc = 0;
    LOG(INFO)<<"start handle rows";
    switch (event.type) {
        case fdemo::slave::LogEvent::WRITE_ROWS_EVENTv2:
        {
            rc = insertSqlHandler(rows);
            break;
        }
        case fdemo::slave::LogEvent::DELETE_ROWS_EVENTv2:
        {
            rc = deleteSqlHandler(rows);
            break;
        }
        case fdemo::slave::LogEvent::UPDATE_ROWS_EVENTv2:
        {
            rc = updateSqlHandler(rows);
            break;
        }
        default:
            break;
    }
    if(rc != 0) {
        LOG(ERROR)<<"handle RowsEvent error,errno:"<<rc;
    }
    return 0;
}

int BinlogSync::deleteSqlHandler(std::vector<fdemo::slave::RowValue> rows) {
    for(std::vector<fdemo::slave::RowValue>::iterator it = rows.begin(); it != rows.end(); it++) {
        std::vector<std::string> tmpWhere;
        for(size_t i = 0; i < it->columns.size(); i++) {
            std::string str = it->columns[i] + " = " + it->beforeValue[i];
            tmpWhere.push_back(str);
        }
        const char* joinchar = " and ";
        std::string whereCluse = strJoin(tmpWhere, joinchar);
        char sql[1024];
        snprintf(sql, sizeof(sql), "delete from %s.%s where %s", it->db.c_str(), it->table.c_str(), whereCluse.c_str());
        LOG(INFO)<<"delete sql is:"<< sql;
    }
    return 0;
}

int BinlogSync::insertSqlHandler(std::vector<fdemo::slave::RowValue> rows) {
    for(std::vector<fdemo::slave::RowValue>::iterator it = rows.begin(); it != rows.end(); it++){
        const char* joinchar = " , ";
        char sql[1024];
        snprintf(sql, sizeof(sql), "insert into %s.%s (%s) values (%s)", it->db.c_str(), it->table.c_str(), strJoin(it->columns, joinchar).c_str(), strJoin(it->afterValue, joinchar).c_str());
        LOG(INFO)<<"insert sql is:"<< sql;
    }
    return 0;
}

int BinlogSync::updateSqlHandler(std::vector<fdemo::slave::RowValue> rows) {
    for(std::vector<fdemo::slave::RowValue>::iterator it = rows.begin(); it != rows.end(); it++){
        std::vector<std::string> beforeJoin;
        std::vector<std::string> afterJoin;
        for(size_t i = 0; i < it->columns.size(); i++) {
            LOG(INFO)<<"i is:"<<i;
            std::string str = it->columns[i] + " = " + it->beforeValue[i];
            std::string str1 = it->columns[i] + " = " + it->afterValue[i];
            beforeJoin.push_back(str);
            afterJoin.push_back(str1);
        }
        const char* joinbefore = " and ";
        const char* joinafter = " , ";
        char sql[1024];
        snprintf(sql, sizeof(sql), "update %s.%s set %s where %s",it->db.c_str(), it->table.c_str(), strJoin(afterJoin, joinafter).c_str(), strJoin(beforeJoin, joinbefore).c_str());
        LOG(INFO)<<"update sql is:"<< sql;
    }
    return 0;
}


std::string BinlogSync::strJoin(std::vector<std::string>& str, const char* joinchar){
    std::stringstream outstr;
    auto begin = str.begin();
    outstr<<*begin;
    if(begin != str.end()) {
        begin++;
        for(auto loop = begin;loop != str.end();loop++) {
            outstr<<joinchar<<*loop;
        }
    }
    LOG(INFO)<<"join str:"<<outstr.str();
    return outstr.str();
}

} // namespace binlogevent
} //namespace fdemo
