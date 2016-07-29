
#include "mockslave/binlogsync.h"

namespace fdemo{
namespace mockslave{

std::string strJoin(std::vector<std::string>& str, const char* joinchar, bool ifValue){
    std::stringstream outstr;
    std::vector<std::string>::iterator begin = str.begin();
    outstr<<*begin;
    if(begin != str.end()) {
        begin++;
        for(std::vector<std::string>::iterator loop = begin;loop != str.end();loop++) {
            if(ifValue){
                outstr<<joinchar<<"'"<<*loop<<"'";
            } else{
                outstr<<joinchar<<*loop;
            }
        }
    }
    //LOG(INFO)<<"join str:"<<outstr.str();
    return outstr.str();
}

//compute primary key's value hash number
size_t getPrikeyHash(fdemo::binlogparse::RowValue* row) {
    if (row->columns.size() == 0) {
        return -1;
    }
    std::string prikeyValueStr;
    std::string prikeyPosValue = row->columns[row->columns.size()-1];
    for(size_t i = 0; i < prikeyPosValue.size();i++) {
        char tmpValue = prikeyPosValue[i];
        int pos;
        if(tmpValue <= '9') {
            pos = tmpValue - '0';
        } else if ((tmpValue >= 'A') && (tmpValue <='Z')) {
            pos = tmpValue - 'A' + 11;
        } else {
            pos = tmpValue - 'a' + 36;
        }
        if(row->beforeValue.size() != 0) {
            prikeyValueStr += row->beforeValue[pos];
        } else {
            prikeyValueStr += row->afterValue[pos];
        }
    }
    size_t str_hash = std::hash<std::string>{} (prikeyValueStr);
    LOG(INFO)<<"hash key:"<<str_hash;
    return str_hash;
}

BinlogSync::BinlogSync(fdemo::utils::XmlConfig xml) {
    meta_.init(xml);
    //tmp,change later
    //meta_.srcMysqlInfo_ = meta_.srcMysqlInfo_;
    pool_ = new fdemo::common::ThreadPool(meta_.poolSize_, meta_.dstMysqlInfo_);
}

BinlogSync::~BinlogSync(){
    pool_->stop();
}

void BinlogSync::run() {
    int rc = 0;
    fdemo::binlogparse::MockSlave reader;
    rc = reader.Connect(meta_.srcMysqlInfo_.host, meta_.srcMysqlInfo_.port, meta_.srcMysqlInfo_.user, meta_.srcMysqlInfo_.passwd);
    if(rc != 0) {
        LOG(ERROR)<<"Connect host:"<<meta_.srcMysqlInfo_.host<<", port:"<<meta_.srcMysqlInfo_.port<<"failed";    
        return;
    }

    rc = reader.DumpBinlog(meta_.srcMysqlInfo_.server_id, meta_.srcMysqlInfo_.default_file, meta_.srcMysqlInfo_.default_offset);
    if(rc != 0) {
        LOG(ERROR)<<"DumpBinlog failed,file:"<<meta_.srcMysqlInfo_.default_file<<", offset:"<<meta_.srcMysqlInfo_.default_offset;
        return;
    }
    rc = reader.run(this);
    if(rc != 0) {
        LOG(ERROR)<<"MockSlave::run failed";
        return;
    }
    LOG(INFO)<<"BinlogSync::run end!";
}

//consider parallel replication
//1. according to table's name to parallel
//2. according to pri key to parallel
//TODO
//int BinlogSync::onRowsEvent(const fdemo::binlogparse::RowsEvent& event, std::vector<fdemo::binlogparse::RowValue>& rows) {
//    //LOG(INFO)<<"start onRowsEvent, event type:"<<event.type;
//    int taskQueneId = event.tableid % meta_.poolSize_;
//    pool_->AddTask2TaskMap(new EventHandler(event, rows), taskQueneId);
//    return 0;
//}

int BinlogSync::onRowsEvent(const fdemo::binlogparse::RowsEvent& event, std::vector<fdemo::binlogparse::RowValue>& rows) {
    //for(std::vector<fdemo::binlogparse::RowValue>::iterator it = rows.begin(); it != rows.end();it++) {
    for(size_t i = 0; i<rows.size(); i++) {
        size_t taskQueneId = getPrikeyHash(&rows[i]) % meta_.poolSize_;
        //LOG(INFO)<<"taskQueneId:"<<taskQueneId;
        pool_->AddTask2TaskMap(new SingleEventHandler(event, rows[i], pool_->getMysqlConnect(taskQueneId)), taskQueneId);
        //LOG(INFO)<<"taskQueneId:"<<taskQueneId<<" ,add success";
    }
    return 0;
}

EventHandler::~EventHandler(){
    rows_.clear();
}

void EventHandler::run() {
    LOG(INFO)<<"start run, event type:"<<event_.type;
    int rc = 0;
    switch (event_.type) {
        case fdemo::binlogparse::LogEvent::WRITE_ROWS_EVENTv2:
        {
            rc = insertSqlHandler();
            break;
        }
        case fdemo::binlogparse::LogEvent::DELETE_ROWS_EVENTv2:
        {
            rc = deleteSqlHandler();
            break;
        }
        case fdemo::binlogparse::LogEvent::UPDATE_ROWS_EVENTv2:
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
    for(std::vector<fdemo::binlogparse::RowValue>::iterator it = rows_.begin(); it != rows_.end(); it++) {
        std::vector<std::string> tmpWhere;
        //attention: it->columns.size() != event_.columncount,because of primary key
        for(size_t i = 0; i < event_.columncount; i++) {
            std::string str = it->columns[i] + " = " + it->beforeValue[i];
            tmpWhere.push_back(str);
        }
        const char* joinchar = " and ";
        std::string whereCluse = strJoin(tmpWhere, joinchar, false);
        char sql[1024];
        snprintf(sql, sizeof(sql), "delete from %s.%s where %s", it->db.c_str(), it->table.c_str(), whereCluse.c_str());
        LOG(INFO)<<"delete sql is:"<< sql;
    }
    return 0;
}

int EventHandler::insertSqlHandler() {
    for(std::vector<fdemo::binlogparse::RowValue>::iterator it = rows_.begin(); it != rows_.end(); it++){
        const char* joinchar = " , ";
        char sql[1024];
        snprintf(sql, sizeof(sql), "insert into %s.%s (%s) values (%s)", it->db.c_str(), it->table.c_str(), strJoin(it->columns, joinchar, false).c_str(), strJoin(it->afterValue, joinchar, false).c_str());
        LOG(INFO)<<"insert sql is:"<< sql;
    }
    return 0;
}

int EventHandler::updateSqlHandler() {
    for(std::vector<fdemo::binlogparse::RowValue>::iterator it = rows_.begin(); it != rows_.end(); it++){
        std::vector<std::string> beforeJoin;
        std::vector<std::string> afterJoin;
        //for(size_t i = 0; i < it->columns.size(); i++) {
        for(size_t i = 0; i < event_.columncount; i++) {
            std::string str = it->columns[i] + " = " + it->beforeValue[i];
            std::string str1 = it->columns[i] + " = " + it->afterValue[i];
            beforeJoin.push_back(str);
            afterJoin.push_back(str1);
        }
        const char* joinbefore = " and ";
        const char* joinafter = " , ";
        char sql[1024];
        snprintf(sql, sizeof(sql), "update %s.%s set %s where %s",it->db.c_str(), it->table.c_str(), strJoin(afterJoin, joinafter, false).c_str(), strJoin(beforeJoin, joinbefore, false).c_str());
        LOG(INFO)<<"update sql is:"<< sql;
    }
    return 0;
}

void SingleEventHandler::run(){
    //LOG(INFO)<<"start SingleEventHandler, type:"<<event_.type;
    switch(event_.type) {
        case fdemo::binlogparse::LogEvent::UPDATE_ROWS_EVENTv2:
        {
            std::vector<std::string> beforeJoin;
            std::vector<std::string> afterJoin;
            for(size_t i = 0; i < event_.columncount; i++) {
                std::string str = row_.columns[i] + " = '" + row_.beforeValue[i]+ "'";
                std::string str1 = row_.columns[i] + " = '" + row_.afterValue[i]+ "'";
                beforeJoin.push_back(str);
                afterJoin.push_back(str1);
            }
            const char* joinbefore = " and ";
            const char* joinafter = " , ";
            char sql[512];
            int len = snprintf(sql, sizeof(sql), "update %s.%s set %s where %s",row_.db.c_str(), row_.table.c_str(), strJoin(afterJoin, joinafter, false).c_str(), strJoin(beforeJoin, joinbefore, false).c_str());
            LOG(INFO)<<"update sql is:"<< sql;
            if(!(sh_->ExecuteSql(sql, len))){
                LOG(ERROR)<<"execute sql: "<<sql<<" , error";
            }
            break;
        }
        case fdemo::binlogparse::LogEvent::WRITE_ROWS_EVENTv2:
        {
            const char* joinchar = " , ";
            char sql[256];
            //delete the last(primary key)
            row_.columns.pop_back();
            int len = snprintf(sql, sizeof(sql), "insert into %s.%s (%s) values (%s)", row_.db.c_str(), row_.table.c_str(), strJoin(row_.columns, joinchar, false).c_str(), strJoin(row_.afterValue, joinchar, true).c_str());
            LOG(INFO)<<"insert sql is:"<< sql;
            if(!(sh_->ExecuteSql(sql, len))){
                LOG(ERROR)<<"execute sql: "<<sql<<" , error";
            }
            break;
        }
        case fdemo::binlogparse::LogEvent::DELETE_ROWS_EVENTv2:
        {
            std::vector<std::string> tmpWhere;
            for(size_t i = 0; i < event_.columncount; i++) {
                std::string str = row_.columns[i] + " = '" + row_.beforeValue[i] + "'";
                tmpWhere.push_back(str);
            }
            const char* joinchar = " and ";
            std::string whereCluse = strJoin(tmpWhere, joinchar, false);
            char sql[256];
            int len = snprintf(sql, sizeof(sql), "delete from %s.%s where %s", row_.db.c_str(), row_.table.c_str(), whereCluse.c_str());
            LOG(INFO)<<"delete sql is:"<< sql;
            if(!(sh_->ExecuteSql(sql, len))){
                LOG(ERROR)<<"execute sql: "<<sql<<" , error";
            }
            break;
        }
        default:
            LOG(INFO)<<"default event type:"<<event_.type;
            break;
    }
}

} // namespace mockslave
} //namespace fdemo
