//Author: demon1991yl@gmail.com

#ifndef FDEMO_SLAVE_MOCK_SLAVE_H_
#define FDEMO_SLAVE_MOCK_SLAVE_H_

#include "metadata.h"
#include "glog/logging.h"
#include <mysql/mysql.h>
#include <mysql/my_global.h>
#include <mysql/m_ctype.h>
#include <mysql/sql_common.h>
#include <mysql/mysql_com.h>

struct st_mysql;
struct st_mysql_res;

namespace fdemo{
namespace slave{

class MockSlave {

public:
    MockSlave(){}
    ~MockSlave(){}

    int Connect(const std::string& host, int port, const std::string& user, const std::string& passwd);
    int DumpBinlog(uint32_t ServerId, const std::string& filename, uint32_t offset);
    int run();
    void Close();

private:
    int readEvent();
    int processEvent();

private:
    st_mysql* slave_;
    st_mysql* schema_;

    uint32_t ServerId_;
    std::string filename_;
    uint32_t offset_;
    //struct BinlogInfo master_info_;
};

} // namespace slave
} // namespace fdemo


#endif
