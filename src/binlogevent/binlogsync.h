#ifndef FDEMO_BINLOGEVENT_BINLOGSYNC_H_
#define FDEMO_BINLOGEVENT_BINLOGSYNC_H_

#include "slave/mock_slave.h"
#include "slave/metadata.h"
#include "common/threadpool.h"
#include "common/thread.h"
#include <vector>

namespace fdemo{
namespace binlogevent{

class BinlogSync: public fdemo::slave::EventAction {
//class BinlogSync: public fdemo::slave::EventAction {
public:
    BinlogSync(fdemo::slave::BinlogInfo& info, int poolsize);
    virtual ~BinlogSync();
    virtual void run();

    virtual int onRowsEvent(const fdemo::slave::RowsEvent& event, std::vector<fdemo::slave::RowValue> rows);
    //virtual int onQueryEvent();

private:
    fdemo::slave::BinlogInfo master_info_;
    fdemo::common::ThreadPool* pool_;
    int size_;
};

class EventHandler: public fdemo::common::Runable {
public:
    EventHandler(const fdemo::slave::RowsEvent& event ,std::vector<fdemo::slave::RowValue> rows) : rows_(rows), event_(event) {}
    virtual ~EventHandler() {}
    virtual void run();
    int updateSqlHandler();
    int deleteSqlHandler();
    int insertSqlHandler();
private:
    std::string strJoin(std::vector<std::string>& str, const char* joinchar);

private:
    std::vector<fdemo::slave::RowValue> rows_;
    const fdemo::slave::RowsEvent event_; //not const fdemo::slave::RowsEvent& event_;

};

} //namespace binlogevent
} //namespace fdemo
#endif
