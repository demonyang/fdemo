//Author: demon1991yl@gmail.com

#ifndef FDEMO_SLAVE_METADATA_H_
#define FDEMO_SLAVE_METADATA_H_

#include <vector>
#include <string>
#include <set>

namespace fdemo {
namespace slave {

struct BinlogInfo {
    std::string name;
    std::string host;
    int port;
    std::string user;
    std::string passwd;
    std::string default_file;
    uint32_t default_offset;
    uint32_t server_id;
};

class Metadata {
};

} // namespace slave
} //namespace fdemo

#endif
