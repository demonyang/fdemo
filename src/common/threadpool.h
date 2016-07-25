#ifndef FDEMO_COMMON_THREADPOOL_H_
#define FDEMO_COMMON_THREADPOOL_H_

#include<deque>
#include<map>
#include<vector>
#include<assert.h>
#include "common/mutex.h"
#include "common/thread.h"
#include "glog/logging.h"

namespace fdemo{
namespace common{

class ThreadPool {

public:
    ThreadPool(int thread_num = 10);
    ~ThreadPool();

    size_t AddTask(Runable* func);
    size_t AddTask2TaskMap(Runable* func, int taskMapId);
    void stop();
    size_t size();
    Runable* take(int taskQueneId);

private:
    ThreadPool(const ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);

    void createThreads();
    static void *proc(void *arg);


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
};

struct TakeTask {
    ThreadPool*  pool;
    int    taskQueneId;
};

} //namespace common
} //namespace fdemo

#endif
