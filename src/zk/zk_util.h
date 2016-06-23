//Author:demon1991yl@gmail.com

#ifndef DEMON_ZK_UTIL_H_
#define DEMON_ZK_UTIL_H_

namespace fdemo{
namespace zk{
enum ZooKeeperState {
    ZS_DISCONN,
    ZS_CONNECTING,
    ZS_CONNECTED,
    ZS_AUTH,
    ZS_TIMEOUT
}; 

} //namespace zk
} //namespace fdemo
#endif
