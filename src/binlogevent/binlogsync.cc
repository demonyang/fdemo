
#include "binlogevent/binlogsync.h"

namespace fdemo{
namespace binlogevent{

BinlogSync::BinlogSync(fdemo::slave::BinlogInfo& info, int poolsize) {
    master_info_ = info;
    pool_ = new fdemo::common::ThreadPool(poolsize);
    size_ = poolsize;
}

BinlogSync::~BinlogSync(){
    pool_->stop();
}

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
    //LOG(INFO)<<"start onRowsEvent, event type:"<<event.type;
    int taskQueneId = event.tableid % size_;
    pool_->AddTask2TaskMap(new EventHandler(event, rows), taskQueneId);
    return 0;
}

void EventHandler::run() {
    LOG(INFO)<<"start run, event type:"<<event_.type;
    int rc = 0;
    switch (event_.type) {
        case fdemo::slave::LogEvent::WRITE_ROWS_EVENTv2:
        {
            rc = insertSqlHandler();
            break;
        }
        case fdemo::slave::LogEvent::DELETE_ROWS_EVENTv2:
        {
            rc = deleteSqlHandler();
            break;
        }
        case fdemo::slave::LogEvent::UPDATE_ROWS_EVENTv2:
        {
            rc = updateSqlHandler();
            break;
        }
        default:
        {
            LOG(INFO)<<"default event type:"<<event_.type;
            break;
        }
    }
    if(rc != 0) {
        LOG(ERROR)<<"handle RowsEvent error,errno:"<<rc;
    }
    return;
}

int EventHandler::deleteSqlHandler() {
    for(std::vector<fdemo::slave::RowValue>::iterator it = rows_.begin(); it != rows_.end(); it++) {
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

int EventHandler::insertSqlHandler() {
    for(std::vector<fdemo::slave::RowValue>::iterator it = rows_.begin(); it != rows_.end(); it++){
        const char* joinchar = " , ";
        char sql[1024];
        snprintf(sql, sizeof(sql), "insert into %s.%s (%s) values (%s)", it->db.c_str(), it->table.c_str(), strJoin(it->columns, joinchar).c_str(), strJoin(it->afterValue, joinchar).c_str());
        LOG(INFO)<<"insert sql is:"<< sql;
    }
    return 0;
}

int EventHandler::updateSqlHandler() {
    for(std::vector<fdemo::slave::RowValue>::iterator it = rows_.begin(); it != rows_.end(); it++){
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

std::string EventHandler::strJoin(std::vector<std::string>& str, const char* joinchar){
    std::stringstream outstr;
    std::vector<std::string>::iterator begin = str.begin();
    outstr<<*begin;
    if(begin != str.end()) {
        begin++;
        for(std::vector<std::string>::iterator loop = begin;loop != str.end();loop++) {
            outstr<<joinchar<<*loop;
        }
    }
    //LOG(INFO)<<"join str:"<<outstr.str();
    return outstr.str();
}

} // namespace binlogevent
} //namespace fdemo
