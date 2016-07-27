#ifndef FDEMO_BINLOGEVENT_BINLOGSYNC_H_
#define FDEMO_BINLOGEVENT_BINLOGSYNC_H_

#include "binlogparse/mock_slave.h"
#include "binlogparse/metadata.h"
#include "common/threadpool.h"
#include "common/thread.h"
#include <vector>

namespace fdemo{
namespace binlogevent{

class BinlogSync: public fdemo::slave::EventAction {
public:
    BinlogSync(fdemo::utils::XmlConfig xml);
    virtual ~BinlogSync();
    virtual void run();

    virtual int onRowsEvent(const fdemo::slave::RowsEvent& event, std::vector<fdemo::slave::RowValue>& rows);
    //virtual int onQueryEvent();

private:
    fdemo::slave::MysqlMeta meta_;
    fdemo::common::ThreadPool* pool_;
};

class EventHandler: public fdemo::common::Runable {
public:
    EventHandler(const fdemo::slave::RowsEvent& event ,std::vector<fdemo::slave::RowValue> rows) : rows_(rows), event_(event) {}
    virtual ~EventHandler();
    virtual void run();
    int updateSqlHandler();
    int deleteSqlHandler();
    int insertSqlHandler();

private:
    std::vector<fdemo::slave::RowValue> rows_;
    const fdemo::slave::RowsEvent event_; //not const fdemo::slave::RowsEvent& event_;

};

//parallel by primary key
class SingleEventHandler: public fdemo::common::Runable {
public:
    SingleEventHandler(const fdemo::slave::RowsEvent& event ,fdemo::slave::RowValue row) : row_(row), event_(event) {}
    virtual ~SingleEventHandler() {}
    virtual void run();

private:
    fdemo::slave::RowValue row_;
    const fdemo::slave::RowsEvent event_; //not const fdemo::slave::RowsEvent& event_;
};

} //namespace binlogevent
} //namespace fdemo
#endif
