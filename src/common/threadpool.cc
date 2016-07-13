//Author: demon1991yl@gmail.com

#include "common/threadpool.h"

namespace fdemo{
namespace common{

ThreadPool::ThreadPool(int thread_num){
    Maxnum_ = thread_num;
    IsRunning_ = true;
    for(int i = 0;i<thread_num;i++){
        std::pair<int, std::deque<Runable*>> kv;
        kv.first = i;
        task_map_.insert(kv);
    }
    createThreads();
}
ThreadPool::~ThreadPool() {
    stop();
    for(std::map<int,std::deque<Runable*>>::iterator loop = task_map_.begin(); loop != task_map_.end(); loop++){
        for(std::deque<Runable*>::iterator loop1 = loop->second.begin(); loop1 != loop->second.end();loop1++) {
            delete *loop1;
        }
        loop->second.clear();
    }
}

void ThreadPool::createThreads() {
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&condition_, NULL);
    threads_ = (pthread_t*)malloc(sizeof(pthread_t) * Maxnum_);
    for(int i = 0;i<Maxnum_;i++) {
        TakeTask* threadRrg = new TakeTask{this, i};
        LOG(INFO)<<"taketask i:"<<threadRrg->taskQueneId;
        pthread_create(&threads_[i], NULL, &ThreadPool::proc, (void*)threadRrg );
    }
}

size_t ThreadPool::AddTask(Runable* func) {
    MutexLock mutex(&state_lock_);
    task_list_.push_back(func);
    int size = task_list_.size();
    pthread_cond_signal(&condition_);
    return size;
}

size_t ThreadPool::AddTask2TaskMap(Runable* func, int taskMapId){
    MutexLock mutex(&state_lock_);
    std::map<int, std::deque<Runable*>>::iterator it = task_map_.find(taskMapId);
    if(it == task_map_.end()) {
        return -1;
    }
    it->second.push_back(func);
    int size = it->second.size();
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
    int size = 0;
    for(std::map<int, std::deque<Runable*>>::iterator it = task_map_.begin(); it != task_map_.end();it++){
        size += it->second.size();
    }
    return size;
}

Runable* ThreadPool::take(int taskQueneId) {
    Runable * task = NULL;
    std::map<int, std::deque<Runable*>>::iterator taskQuene = task_map_.find(taskQueneId);
    while(!task) {
        pthread_mutex_lock(&mutex_);
        //while (IsRunning_ && taskQuene->second.empty()) {
        //    LOG(INFO)<<"pthread_cond_wait, taskQueneId:"<<taskQueneId;
        //    //pthread_cond_wait()->unlock()->lock()
        //    pthread_cond_wait(&condition_, &mutex_);
        //}
        if (!IsRunning_) {
            pthread_mutex_unlock(&mutex_);
            break;

        } else if (taskQuene->second.empty()) {
            pthread_mutex_unlock(&mutex_);
            continue;
        }
        assert(!taskQuene->second.empty());
        task = taskQuene->second.front();
        taskQuene->second.pop_front();
        pthread_mutex_unlock(&mutex_);
    }
    return task;
}

void* ThreadPool::proc(void *arg) {
    pthread_t tid = pthread_self();
    TakeTask *takeArg = static_cast<TakeTask*>(arg);
    LOG(INFO)<<"i'am tid:"<<tid<<" ,taskid:"<<takeArg->taskQueneId;
    while(takeArg->pool->IsRunning_) {
        Runable * run_func = takeArg->pool->take(takeArg->taskQueneId);
        if(!run_func) {
            LOG(ERROR)<<tid<<":pool take is null";
            continue;
            //break;
        }
        assert(run_func);
        run_func->run();
    }
    delete takeArg;
    return NULL;
}

} //namespace common
} //namespace fdemo
