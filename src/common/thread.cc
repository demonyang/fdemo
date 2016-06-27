// Author:demon1991yl@gmail.com

#include "thread.h"

namespace fdemo {
namespace common {

void FuncWrap::run() {
    (*func_)();    
}

bool Thread::join() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&tid_, &attr, &Thread::proc, runable_);
    void *status;
    int ret = pthread_join(tid_, &status);
    if(ret != 0) {
        LOG(ERROR)<< "pthread_join fail, ret code:" << ret;
        return false;
    } else {
        LOG(INFO)<< "pthread_join status:"<< (long)status;
    }
    return true;
    
}

} // namespace common
} // namespace fdemo
