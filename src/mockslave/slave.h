//Author: demon1991yl@gmail.com
#ifndef FDEMO_MOCKSLAVE_SLAVE_H
#define FDEMO_MOCKSLAVE_SLAVE_H

#include <mysql/mysql.h>
//#include <mysql/my_global.h>
#include <string>
#include "glog/logging.h"
#include "binlogparse/metadata.h"

namespace fdemo{
namespace mockslave{

class SlaveHandler{
public:
    SlaveHandler(const fdemo::binlogparse::SlaveInfo& slaveinfo);
    ~SlaveHandler(){ Close(); }

    int Connect(const std::string& host, int port, const std::string& user, const std::string& passwd);
    bool ExecuteSql(const char* sql, int len);
    void Close();


private:
    st_mysql* conn_;
};

} // namespace mockslave
} // namespace fdemo

#endif
