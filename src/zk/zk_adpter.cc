//Author: demon1991yl@gmail.com

#include "zk/zk_adpter.h"
#include "glog/logging.h"
#include <errno.h>
#include <cstdint>

namespace fdemo{
namespace zk{

ZooKeeperAdapter::ZooKeeperAdapter()
    : zk_handle_(NULL),zk_state_(ZS_DISCONN) {}

ZooKeeperAdapter::~ZooKeeperAdapter() {
    if (NULL != zk_handle_) {
    }
}

bool ZooKeeperAdapter::Init(const std::string& server_list, const std::string& root_path, uint32_t session_timeout, const std::string& id, int* zk_errno) {
    //init zk
    MutexLock mutex(&state_mutex_);
    if (NULL != zk_handle_) {
        return false;
    }
    zk_server_list_ = server_list;
    zk_root_path_ = root_path;
    if (*zk_root_path_.end() == '/') {
        zk_root_path_.resize(zk_root_path_.size()-1);
    }
    zk_id_ = id;

    zk_handle_ = zookeeper_init((zk_server_list_+zk_root_path_).c_str(), NULL, session_timeout, NULL, this, 0);
    if (zk_handle_ == NULL) {
        LOG(ERROR) << "zookeeper_init fail :" <<zerror(errno);
        return false;
    }
    return true;
}

bool ZooKeeperAdapter::Create(const std::string& path, const std::string& value, int flag, std::string* ret_path, int* zk_errno) {
    // lock first
    // TODO

    //check path if vaild
    if (path.empty() || path[0] != '/' || (path.size() >1 && *path.rbegin() == '/')) {
        return false;
    }

    //check if connected
    if (zk_handle_ == NULL) {
        return false;
    }

    int value_len = value.size();
    if (value_len == 0) {
        value_len = -1;
    }

    size_t root_path_len = zk_root_path_.size();
    size_t path_len = path.size();
    char * ret_path_buf = NULL;
    size_t ret_path_size = 0;
    //ret_path is a pointer
    if (ret_path != NULL) {
        ret_path_size = root_path_len + path_len + 11;
        ret_path_buf = new char[ret_path_size];
    }
    int ret = zoo_create(zk_handle_, path.c_str(), value.c_str(), value_len, &ZOO_OPEN_ACL_UNSAFE, flag, ret_path_buf, ret_path_size);
    if (ret == ZOK) {
        if (NULL != ret_path) {
            size_t ret_path_len = strlen(ret_path_buf);
            if (((flag & ZOO_SEQUENCE) ==1 && ret_path_len == root_path_len+path_len+10) ||
                    ((flag & ZOO_SEQUENCE) == 0 && ret_path_len == root_path_len+path_len)
                    ){
                // +root_path_len equal move right
                *ret_path = ret_path_buf + root_path_len;
            } else {
                *ret_path = ret_path_buf;
            }
        }
        LOG(INFO) << "zoo_create success";
    } else {
        // log
        LOG(WARNING) << "zoo_create fail:";
        return false;
    }
    if (NULL != ret_path_buf) {
        delete[]  ret_path_buf;
    }
    return true;
}

bool ZooKeeperAdapter::CreatePersistentNode(const std::string& path, const std::string& value, int * zk_errno){
    //lock first
    MutexLock mutex(&state_mutex_);
    return Create(path, value, 0, NULL, zk_errno);
}
} //namespace zk
} //namespace fdemo
