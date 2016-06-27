//Author:demon1991yl@gmail.com

#ifndef FDEMO_COMMON_THREAD_H_
#define FDEMO_COMMON_THREAD_H_

#include "pthread.h"
#include "glog/logging.h"
#include <cstdio>

namespace fdemo{
namespace common{

class Runable {
public:
    Runable() {};
    virtual ~Runable() {};
    virtual void run() = 0;
};

//for print test func
class FuncWrap: public Runable {
public:
    FuncWrap(void (*func)()): func_(func) {};
    virtual ~FuncWrap() {};
    virtual void run();
private:
    void (*func_)();

};

class Thread {

    static void * proc(void *arg) {
        Runable *r = static_cast<Runable*>(arg); //cast(强制转换) arg to Runable*
        r->run();
        delete r;
        return NULL;
    }

public:
    Thread(Runable *r): runable_(r) {};
    ~Thread(){};

    void start() {
        pthread_create(&tid_, NULL, &Thread::proc, runable_);
    }

    void detach() {
        pthread_detach(tid_);
    }

    bool join();

    static void schedule(Runable* func) {
        Thread * thread =  new Thread(func);
        bool ret = thread->join();
        if (!ret) {
            LOG(ERROR)<<"schedule fail";
        }
        delete thread;
    }
private:
    pthread_t tid_;
    Runable *runable_;
};

class PrintFunc: public Runable {
public:
    PrintFunc() {};
    virtual ~PrintFunc(){};
    virtual void run() {
        printf("hello world!");
    }
};


} // namespace common
} //namespace common

#endif
