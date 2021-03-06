//Author: demon1991yl@gmail.com

#include "binlogparse/mock_slave.h"
#include "string.h"
#include <iostream>

//#define cli_send_command(mysql, cmd, arg, length) cli_advanced_command(mysql, cmd, NULL, 0, arg, length, 0, NULL)

namespace fdemo{
namespace binlogparse{

int MockSlave::Connect(const std::string& host, int port, const std::string& user, const std::string& passwd){
    //two connections
    mysql_thread_init();

    if ((slave_ = mysql_init(NULL)) == NULL) {  //one connect for read binlog
        return -1;
    }

    if ((schema_ = mysql_init(NULL)) == NULL) {  //one connect for query schema
        return -1;
    }

    if ((mysql_real_connect(slave_, host.c_str(), user.c_str(), passwd.c_str(), NULL, port, NULL, 0)) == NULL) {
        LOG(ERROR)<<"mysql_real_connect error,host:"<<host.c_str()<<"port:"<<port<<"reason:"<<mysql_error(slave_);
        return -1;
    }

    if ((mysql_real_connect(schema_, host.c_str(), user.c_str(), passwd.c_str(), NULL, port, NULL, 0)) == NULL) {
        LOG(ERROR)<<"mysql_real_connect error,host:"<<host.c_str()<<"port:"<<port<<"reason:"<<mysql_error(schema_);
        return -1;
    }
    LOG(INFO)<<"connect mysql success,host:"<<host.c_str()<<", port:"<<port;
    return 0;
}

int MockSlave::DumpBinlog(uint32_t ServerId, const std::string& filename, uint32_t offset) {
    //after mysql5.6, before send COM_BINLOG_DUMP, should send the checksum to master
    if(setCrc32() != 0) {
        return -1;
    }
    unsigned char buf[256];
    unsigned char *ptr = buf;
    ServerId_ = ServerId;
    filename_  = filename;
    offset_ = offset;
    int4store(ptr, offset); //binlog-pos
    ptr += 4;
    int2store(ptr, 0); // 0:represent block when no more event,1:represet non-block,return EOF_packet
    ptr += 2;
    int4store(ptr, ServerId); // server-id
    ptr += 4;
    memcpy(ptr, filename.data(), filename.size());
    ptr += filename.size();
    //const unsigned char * tmpptr = NULL;
    if(cli_advanced_command(slave_, COM_BINLOG_DUMP, NULL, 0, buf, ptr - buf, 0, NULL) != 0) {
       LOG(ERROR)<<"cli_advanced_command error, reason:"<< mysql_error(slave_);
       return mysql_errno(slave_);
    } 

    if(cli_safe_read(slave_) == 0) {
       LOG(ERROR)<<"cli_safe_read error,file:"<<filename.c_str()<<"offset:"<<offset<<"errno:err"<<mysql_errno(slave_)<<":"<<mysql_error(slave_);
       return mysql_errno(slave_);
    }

    if(slave_->net.read_pos[0] != '\0') {
        LOG(ERROR)<< "read_pos error, reason:"<< mysql_error(slave_);
       return mysql_errno(slave_);
    }

    return 0;
}


int MockSlave::run(EventAction* eventaction) {
    int rc = -1;
    LogEvent header;
    ByteArray body;
    while(1) {
        if((rc = readEvent(&header, &body)) != 0){
            break;
        }
        if((rc = processEvent(header, body, eventaction)) != 0){
            break;
        }
        //break;
    }
    return rc;
}

int MockSlave::readEvent(LogEvent* header, ByteArray* body) {
    //cli_safe_read() get one binlog event
    unsigned long len = cli_safe_read(slave_);
    LOG(INFO)<<"readEvent len:"<<len;
    if(len == packet_error || len == 0) {
        LOG(ERROR)<<"cli_safe_read error len:"<<len<<"errno:err"<<mysql_error(slave_)<<":"<<mysql_errno(slave_);
        return mysql_errno(slave_);
    }
    //EOF packet
    //if (slave_->net.read_pos[0] == 254 && len < 9) {
    //    return ERR_EOF;
    //}
    if (slave_->net.read_pos[0] !=0) {
        LOG(ERROR)<<"read_pos[0] errno:err"<<mysql_error(slave_)<<":"<<mysql_errno(slave_);
        return mysql_errno(slave_);
    }
    //network streams are request with COM_BINLOG_DUMP and prepend each Binlog Event with 00 OK-byte
    // newlen = len - 1(00) - 4(checksum) 
    body->assign((char*)slave_->net.read_pos+1, len -5);
    header->unpack(*body);
    return 0;
}

int MockSlave::processEvent(LogEvent header, ByteArray body, EventAction* eventaction) {
    if(header.type != LogEvent::ROTATE_EVENT && header.logpos <= offset_) {
        LOG(ERROR)<<"processEvent err, current position:"<<offset_<<"event position:"<<header.logpos<<"event type:"<<header.type<<",server-id"<<header.server_id;
        return -1;
    }
    LOG(INFO)<<"processEvent, current position:"<<offset_<<"event position:"<<header.logpos<<"event type:"<<header.type<<",server-id"<<header.server_id<<",position:"<<body.position();
    int rc;
    try{
        switch (header.type) {
            case LogEvent::ROTATE_EVENT:
            {
                RotateEvent event(header);
                event.unpack(body);
                rc = onRotateEvent(event);
                break;
            }
            case LogEvent::TABLE_MAP_EVNET:
            {
                TableMapEvent event(header);
                event.unpack(body);
                rc = onTableMapEvent(event);
                break;
            }
            case LogEvent::WRITE_ROWS_EVENTv2:
            case LogEvent::UPDATE_ROWS_EVENTv2:
            case LogEvent::DELETE_ROWS_EVENTv2:
            {
                RowsEvent event(header); 
                event.unpack(body);
                rc = onRowsEvent(event, eventaction);
                break;
            }
            default:
                rc = 0;
                offset_ = header.logpos;
                break;
        }

    } catch(MalformException& e){
        LOG(ERROR)<<"processEvent exception:"<<e.what();
        return -1;
    }
    if(rc != 0) {
        LOG(ERROR)<<"processEvent error,rc:"<<rc;
    }
    return rc;
}

int MockSlave::onRotateEvent(const RotateEvent& event){
    filename_ = event.filename;
    offset_ = event.position;
    return 0;
}

char to62Letter(int pos) {
    if(pos<=10) {
        return '0' + pos-1;
    } else if (pos>10 && pos <=36){
        return 'A' + pos-11;
    } else {
        return 'a' + pos-36;
    }
}

int MockSlave::onTableMapEvent(const TableMapEvent& event) {
    LOG(INFO)<<"TableMapEvent:"<<event.tableid;
    std::map<uint64_t, TableSchema*>::iterator tmpSchema = tables_.find(event.tableid);
    if(tmpSchema != tables_.end()) { //exist a table, 1.skip,2.update
        std::string tmpSchemaDbTable = tmpSchema->second->getDBname()+ '.' +tmpSchema->second->getTablename();
        std::string tmpName = event.dbname + '.' + event.tablename;
        if (tmpName == tmpSchemaDbTable) {
        //table-id,db,tablen all same, skip
            return 0;
        } else {
        //update exist schema
            delete tmpSchema->second;
            tables_.erase(tmpSchema);
        }
    } else {
        for(std::map<uint64_t, TableSchema*>::iterator it = tables_.begin();it != tables_.end();it++){
            std::string tmpName = event.dbname+'.'+event.tablename;
            if(tmpName == (it->second->getDBname()+'.'+it->second->getTablename())) {
                delete it->second;
                tables_.erase(it);
                break;
            }
        }
    }

    char sql[1024];
    int len  = snprintf(sql, sizeof(sql), "SELECT COLUMN_NAME,COLUMN_TYPE,CHARACTER_OCTET_LENGTH,ORDINAL_POSITION, COLUMN_KEY FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = '%s' AND TABLE_NAME = '%s' ORDER BY ORDINAL_POSITION", event.dbname.c_str(), event.tablename.c_str());

    MYSQL_RES *res = query(sql, len);
    if (res == NULL) {
        LOG(ERROR)<<"query table schema error, db:"<<event.dbname.c_str()<<" ,table:"<<event.tablename.c_str()<<" ,offset_:"<<event.logpos;
        return -1;
    }

    if(mysql_num_rows(res) == 0) {
        LOG(ERROR)<<"query table schema res 0 rows";
        return -1;
    }

    //add new table schema to tables_
    std::pair<uint64_t, TableSchema*> kv;
    kv.first = event.tableid;
    kv.second = new TableSchema(event.dbname, event.tablename);
    //attention &
    std::string& prikey = kv.second->getPrikey();
    MYSQL_ROW row;
    int row_pos = 0;
    while((row = mysql_fetch_row(res))) {
        while(!kv.second->createField(row[0], row[1], row[2])) {
            LOG(ERROR)<<"schema:"<<event.dbname.c_str()<<'.'<<event.tablename.c_str()<<"field not support";
            delete kv.second;
            return true;
        }
        //for mysql 5.6 or later
        if(strcmp(row[1], "datetime") == 0) {
            FieldDatetime::setColumnMeta((int)event.columnmeta[row_pos]);
        }
        row_pos++;
        if(row[4] != NULL && strcmp(row[4], "PRI") == 0) {
            prikey.push_back(to62Letter(atoi(row[3])));
        }
    }
    mysql_free_result(res);
    tables_.insert(kv);
    offset_ = event.logpos;
    for(std::map<uint64_t, TableSchema*>::iterator it = tables_.begin();it != tables_.end();it++) {
        LOG(INFO)<<"table-id:"<<it->first<<" ,db:"<<it->second->getDBname()<<" ,table:"<<it->second->getTablename();
    }

    return 0;
}

st_mysql_res* MockSlave::query(const char* sql, int len) {
    if(mysql_real_query(schema_, sql, len) != 0) {
        return NULL;
    }
    MYSQL_RES *res = mysql_store_result(schema_);
    return res;
}

int MockSlave::onRowsEvent(const RowsEvent& event, EventAction* eventaction) {
    std::map<uint64_t, TableSchema*>::iterator it = tables_.find(event.tableid);
    if(it == tables_.end()){
         return 0;
    }
    std::vector<RowValue> rows;
    TableSchema * table = it->second;
    LOG(INFO)<<"row tableid:"<< event.tableid<<" ,columncount:"<<event.columncount;
    ByteArray bufByte;
    bufByte.assign((char*)event.values.data(), event.values.size());
    while(bufByte.reamin() > 0) {
        try{
            RowValue onerow;
            onerow.rowType = event.type;
            onerow.db = table->getDBname();
            onerow.table = table->getTablename();
            if(event.type == LogEvent::WRITE_ROWS_EVENTv2){
                unpackRow(&onerow, RowAfter, event, bufByte, table);
            } else if(event.type == LogEvent::UPDATE_ROWS_EVENTv2){
                unpackRow(&onerow, RowBefore, event, bufByte, table);
                unpackRow(&onerow, RowAfter, event, bufByte, table);
            } else if(event.type == LogEvent::DELETE_ROWS_EVENTv2){
                unpackRow(&onerow, RowBefore, event, bufByte, table);
            }
            rows.push_back(onerow);
        } catch (MalformException &e) {
            LOG(ERROR)<<"unpackRow table:"<<table->getTablename().c_str()<<" ,exception:"<<e.what();
            offset_ = event.logpos;
            return -1;
        }
    }
    if(eventaction->onRowsEvent(event, rows) != 0) {
        //for test, when in produce environment,should continue
        //return 0;
        LOG(ERROR)<<"onRowsEvent failed";
        return -1;
    }
    offset_ = event.logpos;
    return 0;
}

void MockSlave::unpackRow(RowValue* row, RowValueType rvt, const RowsEvent& event, const ByteArray& bytes, TableSchema* table){

    bool filled = (row->columns.empty() ? false : true);
    //every row start with Bit Map
    BitMap bitmap;
    bitmap.unpack((int)event.columncount, bytes);
    for(uint64_t i =0; i<event.columncount;i++) {
        Field *column_field = NULL;
        std::string value;

        column_field = table->getFieldByIndex(i);
        if(column_field == NULL) {
            std::string errmsg = "getFieldByIndex is null";
            LOG(ERROR)<<"unpackRow err:"<<errmsg;
            throw MalformException(errmsg);
        }
        //LOG(INFO)<<"i:"<<i<<" ,BitMap:"<<bitmap.isSet(i);

        if(!bitmap.isSet(i)) {
            value = column_field->valueString(bytes);
        } else {
            value = column_field->valueDefault();
        }
        //LOG(INFO)<<"bit map value:"<<value;
        //avoid add field to row->columns twice
        if(!filled){
            row->columns.push_back(column_field->fieldName());
        }
        if(rvt == RowBefore) {
            row->beforeValue.push_back(value);
        } else if(rvt == RowAfter) {
            row->afterValue.push_back(value);
        }
    }
    if(!filled) {
        //add primary key to columns last
        row->columns.push_back(table->getPrikey());
    }
}

//mysql5.6 or later
int MockSlave::setCrc32() {
    const char* sql = "SET @master_binlog_checksum=@@global.binlog_checksum";
    
    if (mysql_real_query(slave_, sql, strlen(sql)) != 0) {
        return -1;
    }
    const char* sql2 = "SELECT @master_binlog_checksum";
    if (mysql_real_query(slave_, sql2, strlen(sql2)) != 0) {
        return -1;
    }
    MYSQL_RES *res = mysql_store_result(slave_);
    if(mysql_num_rows(res) == 0) {
        LOG(ERROR)<<"query table schema res 0 rows";
        return -1;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if ((strncmp(row[0], "NONE", 10) != 0) && (strncmp(row[0], "CRC32", 10) != 0) ){
        LOG(ERROR)<<"master_binlog_checksum error, res:"<<row[0];
        return -1;
    }
    mysql_free_result(res);
    LOG(INFO)<<"set crc32 success";

    return 0;
}

void MockSlave::Close() {
    for(std::map<uint64_t, TableSchema*>::iterator it = tables_.begin();it != tables_.end();++it) {
        delete it->second;
    }
    tables_.clear();

    if(slave_ != NULL) {
        mysql_close(slave_);
        slave_ = NULL;
    }

    if(schema_ != NULL) {
        mysql_close(schema_);
        schema_ = NULL;
    }

    mysql_thread_end();
    LOG(INFO)<<"close MockSlave's mysql";
}

} // namespace binlogparse
} //namespace fdemo
