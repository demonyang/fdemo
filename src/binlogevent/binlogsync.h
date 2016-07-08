#ifndef FDEMO_BINLOGEVENT_BINLOGSYNC_H_
#define FDEMO_BINLOGEVENT_BINLOGSYNC_H_

#include "slave/mock_slave.h"
#include "slave/metadata.h"
#include "common/thread.h"

namespace fdemo{
namespace binlogevent{

class BinlogSync: public fdemo::common::Runable {
public:
    BinlogSync(fdemo::slave::BinlogInfo& info): master_info_(info) {}
    virtual ~BinlogSync(){}

    void run();

private:
    fdemo::slave::BinlogInfo master_info_;
};

} //namespace binlogevent
} //namespace fdemo
#endif