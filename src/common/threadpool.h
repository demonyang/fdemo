#ifndef FDEMO_COMMON_THREADPOOL_H_
#define FDEMO_COMMON_THREADPOOL_H_

#include<deque>
#include<assert.h>
#include "common/mutex.h"
#include "common/thread.h"
#include "glog/logging.h"

namespace fdemo{
namespace common{

class MockTask: public Runable {
public:
    MockTask(const std::string& arg):arg_(arg) {}
    virtual void run();
private:
    const std::string arg_;
};

class ThreadPool {

public:
    ThreadPool(int thread_num = 10);
    ~ThreadPool();

    size_t AddTask(Runable* func);
    void stop();
    size_t size();
    Runable* take();

private:
    ThreadPool(const ThreadPool&);
    ThreadPool& operator=(const ThreadPool&);

    void createThreads();
    static void *proc(void *arg);


private:
    pthread_t*  threads_;

    std::deque<Runable*> task_list_;
    Mutex state_lock_;
    int Maxnum_;
    pthread_cond_t condition_;
    pthread_mutex_t mutex_;
    volatile bool IsRunning_;
};

} //namespace common
} //namespace fdemo

#endif
