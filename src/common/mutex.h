#ifndef FDEMO_COMMON_MUTEX_H_
#define FDEMO_COMMON_MUTEX_H_

#include "pthread.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "glog/logging.h"

namespace common {

//only use by self
static void PthreadCall(const char * msg, int result) {
    if (result != 0) {
        fprintf(stderr, "pthread %s:%s\n", msg, strerror(result));
        abort();
    }
}

// a Mutex is an exclusive lock
class Mutex {
public:
    Mutex()
        :owner_(0) {
        pthread_mutexattr_t attr;
        PthreadCall("init mutexattr", pthread_mutexattr_init(&attr));
        PthreadCall("set mutexattr", pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
        PthreadCall("init mutex", pthread_mutex_init(&mu_, &attr));
        PthreadCall("destroy mutexattr", pthread_mutexattr_destroy(&attr));
        }
    ~Mutex() {
        PthreadCall("destroy mutex", pthread_mutex_destroy(&mu_));
    }

    void Lock(const char * msg = NULL, int64_t msg_threshold = 100) {
        pthread_mutex_lock(&mu_);
        //锁住之后一段时间要判断一些状态
        AfterLock(msg, msg_threshold);
    }

    void Unlock() {
        //something todo before unlock
        BeforeLock();
        pthread_mutex_unlock(&mu_);
    }
private:
    void AfterLock(const char* msg, int64_t msg_threshold) {
        owner_ = pthread_self();
    }

    void BeforeLock() {
        owner_ = 0;
    }

private:
    //禁止拷贝构造和复制
    Mutex(const Mutex&);
    void operator=(const Mutex&);
    pthread_mutex_t mu_;
    pthread_t owner_;
};

// Mutex lock guard
class MutexLock {
public:
    explicit MutexLock(Mutex *mu, const char * msg=NULL) :mu_(mu) {
        LOG(INFO)<<"start of lock";
        mu_->Lock(msg);
    }

    ~MutexLock() {
        mu_->Unlock();
        LOG(INFO)<<"end of lock";
    }

private:
    //禁止拷贝构造和复制
    MutexLock(const MutexLock&);
    void operator=(const MutexLock&);
    Mutex *const mu_;
};


}  //namespace common

using common::Mutex;
using common::MutexLock;

#endif
