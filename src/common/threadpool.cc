//Author: demon1991yl@gmail.com

#include "common/threadpool.h"

namespace fdemo{
namespace common{

void MockTask::run() {
    sleep(5);
    std::cout<<"task arg:"<< arg_<<std::endl;
}

ThreadPool::ThreadPool(int thread_num){
    Maxnum_ = thread_num;
    IsRunning_ = true;
    createThreads();
}
ThreadPool::~ThreadPool() {
    stop();
    for (std::deque<Runable*>::iterator it = task_list_.begin();it != task_list_.end();++it){
        delete *it;
    }
    task_list_.clear();
}

void ThreadPool::createThreads() {
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&condition_, NULL);
    threads_ = (pthread_t*)malloc(sizeof(pthread_t) * Maxnum_);
    for(int i = 0;i<Maxnum_;i++) {
        pthread_create(&threads_[i], NULL, &ThreadPool::proc, this);
    }
}

size_t ThreadPool::AddTask(Runable* func) {
    MutexLock mutex(&state_lock_);
    task_list_.push_back(func);
    int size = task_list_.size();
    pthread_cond_signal(&condition_);
    return size;
}

void ThreadPool::stop(){
    MutexLock mutex(&state_lock_);
    if (!IsRunning_) {
        return;
    }

    IsRunning_ = false;
    pthread_cond_broadcast(&condition_);
    for(int i = 0; i<Maxnum_;i++) {
        pthread_join(threads_[i], NULL);
    }
    free(threads_);
    threads_ = NULL;
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&condition_);

}

size_t ThreadPool::size() {
    MutexLock mutex(&state_lock_);
    int size = task_list_.size();
    return size;
}

Runable* ThreadPool::take() {
    Runable * task = NULL;
    while(!task) {
        pthread_mutex_lock(&mutex_);
        while (IsRunning_ && task_list_.empty()) {
            LOG(INFO)<<"pthread_cond_wait";
            //pthread_cond_wait()->unlock()->lock()
            pthread_cond_wait(&condition_, &mutex_);
        }
        if (!IsRunning_) {
            pthread_mutex_unlock(&mutex_);
            break;

        } else if (task_list_.empty()) {
            pthread_mutex_unlock(&mutex_);
            continue;
        }
        assert(!task_list_.empty());
        task = task_list_.front();
        task_list_.pop_front();
        pthread_mutex_unlock(&mutex_);
    }
    return task;
}

void* ThreadPool::proc(void *arg) {
    pthread_t tid = pthread_self();
    ThreadPool *pool = static_cast<ThreadPool*>(arg);
    while(pool->IsRunning_) {
        Runable * run_func = pool->take();
        if(!run_func) {
            LOG(ERROR)<<tid<<":pool take is null";
                break;
        }
        assert(run_func);
        run_func->run();
    }
    return NULL;
}

} //namespace common
} //namespace fdemo
