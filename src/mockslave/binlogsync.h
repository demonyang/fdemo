#ifndef FDEMO_MOCKSLAVE_BINLOGSYNC_H_
#define FDEMO_MOCKSLAVE_BINLOGSYNC_H_

#include "binlogparse/mock_slave.h"
#include "binlogparse/metadata.h"
#include "common/threadpool.h"
#include "common/thread.h"
#include "mockslave/slave.h"
#include <vector>

namespace fdemo{
namespace mockslave{

class BinlogSync: public fdemo::binlogparse::EventAction {
public:
    BinlogSync(fdemo::utils::XmlConfig xml);
    virtual ~BinlogSync();
    virtual void run();

    virtual int onRowsEvent(const fdemo::binlogparse::RowsEvent& event, std::vector<fdemo::binlogparse::RowValue>& rows);
    //virtual int onQueryEvent();

private:
    fdemo::binlogparse::MysqlMeta meta_;
    fdemo::common::ThreadPool* pool_;
};

class EventHandler: public fdemo::common::Runable {
public:
    EventHandler(const fdemo::binlogparse::RowsEvent& event ,std::vector<fdemo::binlogparse::RowValue> rows) : rows_(rows), event_(event) {}
    virtual ~EventHandler();
    virtual void run();
    int updateSqlHandler();
    int deleteSqlHandler();
    int insertSqlHandler();

private:
    std::vector<fdemo::binlogparse::RowValue> rows_;
    const fdemo::binlogparse::RowsEvent event_; //not const fdemo::binlogparse::RowsEvent& event_;

};

//parallel by primary key
class SingleEventHandler: public fdemo::common::Runable {
public:
    SingleEventHandler(const fdemo::binlogparse::RowsEvent& event ,fdemo::binlogparse::RowValue row, SlaveHandler* sh) : row_(row), event_(event), sh_(sh) {}
    virtual ~SingleEventHandler() { delete sh_; }
    virtual void run();

private:
    fdemo::binlogparse::RowValue row_;
    const fdemo::binlogparse::RowsEvent event_; //not const fdemo::binlogparse::RowsEvent& event_;
    SlaveHandler* sh_;
};

} //namespace mockslave
} //namespace fdemo
#endif
