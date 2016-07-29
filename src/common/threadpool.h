#ifndef FDEMO_COMMON_THREADPOOL_H_
#define FDEMO_COMMON_THREADPOOL_H_

#include<deque>
#include<map>
#include<vector>
#include<assert.h>
#include "common/mutex.h"
#include "common/thread.h"
#include "glog/logging.h"
#include "mockslave/slave.h"
#include "binlogparse/metadata.h"

namespace fdemo{
namespace common{

class ThreadPool {

public:
    ThreadPool(int thread_num, const fdemo::binlogparse::SlaveInfo& slaveinfo);
    ~ThreadPool();

    size_t AddTask(Runable* func);
    size_t AddTask2TaskMap(Runable* func, int taskMapId);
    void stop();
    size_t size();
    Runable* take(int taskQueneId);
    fdemo::mockslave::SlaveHandler* getMysqlConnect(int taskMapId);

private:
    ThreadPool(const ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);

    void createThreads();
    static void *proc(void *arg);
    void createMysqlConnects(const fdemo::binlogparse::SlaveInfo& slaveinfo);


private:
    pthread_t*  threads_;

    std::deque<Runable*> task_list_;
    std::map<int, std::deque<Runable*>> task_map_;
    Mutex state_lock_;
    int Maxnum_;
    //pthread_cond_t condition_;
    std::vector<pthread_cond_t> conditions_;
    pthread_mutex_t mutex_;
    volatile bool IsRunning_;

    //fixed nums of mysql connector
    std::vector<fdemo::mockslave::SlaveHandler*> mysqlConnect_;
};

struct TakeTask {
    ThreadPool*  pool;
    int    taskQueneId;
};

} //namespace common
} //namespace fdemo

#endif
