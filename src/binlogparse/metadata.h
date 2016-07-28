//Author: demon1991yl@gmail.com

#ifndef FDEMO_BINLOGPARSE_METADATA_H_
#define FDEMO_BINLOGPARSE_METADATA_H_

#include <vector>
#include <string>
#include <set>
#include "utils/utils_config.h"

namespace fdemo {
namespace binlogparse {

struct BinlogInfo {
    std::string name;
    std::string host;
    int port;
    std::string user;
    std::string passwd;
    std::string default_file;
    uint64_t default_offset;
    uint64_t server_id;
};

struct SlaveInfo{
    std::string host;
    int port;
    std::string user;
    std::string passwd;
};

class MysqlMeta{
public:
    void init(fdemo::utils::XmlConfig& xml);
public:
    BinlogInfo srcMysqlInfo_;
    SlaveInfo dstMysqlInfo_;
    int poolSize_;
    
};

} // namespace binlogparse
} //namespace fdemo

#endif
