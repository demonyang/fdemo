//Author: demon1991yl@gmail.com

#include "slave/mock_slave.h"

#define cli_send_command(mysql, cmd, arg, length) cli_advanced_command(mysql, cmd, NULL, 0, arg, length, 0, NULL)

namespace fdemo{
namespace slave{

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
    }

    if ((mysql_real_connect(schema_, host.c_str(), user.c_str(), passwd.c_str(), NULL, port, NULL, 0)) == NULL) {
        LOG(ERROR)<<"mysql_real_connect error,host:"<<host.c_str()<<"port:"<<port<<"reason:"<<mysql_error(schema_);
    }
    return 0;
}

int MockSlave::DumpBinlog(uint32_t ServerId, const std::string& filename, uint32_t offset) {
    char buf[256];
    char *ptr = buf;
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

int MockSlave::run() {
    int rc = -1;
    while(1) {
        if((rc = readEvent()) != 0){
            break;
        }
        if((rc = processEvent()) != 0){
            break;
        }
    }
    return rc;
}

void MockSlave::Close() {
    if(slave_ != NULL) {
        mysql_close(slave_);
        slave_ = NULL;
    }

    if(schema_ != NULL) {
        mysql_close(schema_);
        schema_ = NULL;
    }

    mysql_thread_end();
}

} // namespace slave
} //namespace fdemo
