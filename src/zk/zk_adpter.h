#ifndef FDEMO_ZK_ADPTER_H_
#define FDEMO_ZK_ADPTER_H_

#include <zookeeper/zookeeper.h>
#include <string>
#include "common/mutex.h"
#include "zk/zk_util.h"

namespace fdemo{
namespace zk {
class ZooKeeperAdapter {
public:
    ZooKeeperAdapter();
    virtual ~ZooKeeperAdapter();
    //Init connection to zookeeper
    bool Init(const std::string& server_list, const std::string& root_path, uint32_t session_timeout, const std::string& id, int* zk_errno);

    bool Create(const std::string& path, const std::string& value, int flag, std::string* ret_path, int* zk_errno);
    bool CreatePersistentNode(const std::string& path, const std::string& value, int * zk_errno);

    bool DeleteNode(const std::string& path, int* zk_errno);

    //
private:
    Mutex state_mutex_;
    zhandle_t * zk_handle_;
    std::string zk_id_;
    std::string zk_root_path_;
    std::string zk_server_list_;
    volatile int zk_state_;
};

} //namespace zk
} // namespace fdemo
#endif
