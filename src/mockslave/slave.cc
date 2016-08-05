//Author: demon1991yl@gmail.com

#include "mockslave/slave.h"

namespace fdemo{
namespace mockslave{

SlaveHandler::SlaveHandler(const fdemo::binlogparse::SlaveInfo& slaveinfo){
    if(Connect(slaveinfo.host, slaveinfo.port, slaveinfo.user, slaveinfo.passwd) != 0) {
        //log
        return;
    }
}

int SlaveHandler::Connect(const std::string& host, int port, const std::string& user, const std::string& passwd){

    //mysql_thread_init();
    if((conn_ = mysql_init(NULL)) == NULL){
        return -1;
    }

    my_bool my_true = true;
    mysql_options(conn_, MYSQL_OPT_RECONNECT, &my_true);

    if(mysql_real_connect(conn_, host.c_str(), user.c_str(), passwd.c_str(), NULL, port, NULL, 0) == NULL) {
        LOG(ERROR)<<"mysql_real_connect error, host: "<<host.c_str()<<" port: "<<port;
        return -1;
    }

    LOG(INFO)<<"connect success";
    return 0;
}

bool SlaveHandler::ExecuteSql(const char* sql, int len){
    LOG(INFO)<<"sql: "<<sql<<" ,len: "<< len;
    if(mysql_real_query(conn_, sql, len) != 0){
        LOG(ERROR)<<"execute err: "<<mysql_error(conn_);
        return false;
    }
    return true;

}

void SlaveHandler::Close(){
    if(conn_ != NULL){
        mysql_close(conn_);
        conn_ = NULL;
    }

    //mysql_thread_end();
}

} // namespace slave
} // namespace fdemo
