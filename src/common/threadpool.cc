//Author: demon1991yl@gmail.com

#include "common/threadpool.h"

namespace fdemo{
namespace common{

ThreadPool::ThreadPool(int thread_num, const fdemo::binlogparse::SlaveInfo& slaveinfo){
    Maxnum_ = thread_num;
    IsRunning_ = true;
    for(int i = 0;i<thread_num;i++){
        std::pair<int, std::deque<Runable*>> kv;
        kv.first = i;
        task_map_.insert(kv);
    }
    createMysqlConnects(slaveinfo);
    createThreads();
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::createThreads() {
    for(int i = 0; i < Maxnum_; i++){
        pthread_cond_t condition;
        pthread_cond_init(&condition, NULL);
        conditions_.push_back(condition);
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, NULL);
        mutexs_.push_back(mutex);
    }
    //pthread_cond_init(&condition_, NULL);
    threads_ = (pthread_t*)malloc(sizeof(pthread_t) * Maxnum_);
    for(int i = 0;i<Maxnum_;i++) {
        TakeTask* threadRrg = new TakeTask{this, i};
        LOG(INFO)<<"taketask i:"<<threadRrg->taskQueneId;
        pthread_create(&threads_[i], NULL, &ThreadPool::proc, (void*)threadRrg );
    }
}

void ThreadPool::createMysqlConnects(const fdemo::binlogparse::SlaveInfo& slaveinfo){
    for(int i = 0; i < Maxnum_; i++){
        mysqlConnect_.push_back(new fdemo::mockslave::SlaveHandler(slaveinfo));      
    }
}

size_t ThreadPool::AddTask(Runable* func) {
    task_list_.push_back(func);
    int size = task_list_.size();
    //pthread_cond_signal(&condition_);
    return size;
}

size_t ThreadPool::AddTask2TaskMap(Runable* func, int taskMapId){
    pthread_mutex_lock(&mutexs_[taskMapId]);
    std::map<int, std::deque<Runable*>>::iterator it = task_map_.find(taskMapId);
    if(it == task_map_.end()) {
        return -1;
    }
    it->second.push_back(func);
    pthread_mutex_unlock(&mutexs_[taskMapId]);
    int size = it->second.size();
    pthread_cond_signal(&conditions_[taskMapId]);
    return size;
}

void ThreadPool::stop(){
    LOG(INFO)<<"in pool stop";
    if (!IsRunning_) {
        return;
    }

    IsRunning_ = false;
    //pthread_cond_broadcast(&condition_);
    for(size_t i = 0; i < conditions_.size(); i++){
        pthread_cond_signal(&conditions_[i]);
    }
    //terminate each thread
    for(int i = 0; i<Maxnum_;i++) {
        pthread_join(threads_[i], NULL);
    }
    free(threads_);
    threads_ = NULL;
    for(size_t i = 0; i < conditions_.size(); i++){
        pthread_cond_destroy(&conditions_[i]);
        pthread_mutex_destroy(&mutexs_[i]);
    }
    //destroy mysql connects
    for(size_t i =0; i< mysqlConnect_.size(); i++){
        delete mysqlConnect_[i];
    }
    //destory task map
    for(std::map<int,std::deque<Runable*>>::iterator loop = task_map_.begin(); loop != task_map_.end(); loop++){
        for(std::deque<Runable*>::iterator loop1 = loop->second.begin(); loop1 != loop->second.end();loop1++) {
            delete *loop1;
        }
        loop->second.clear();
    }
}

size_t ThreadPool::size() {
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
        pthread_mutex_lock(&mutexs_[taskQueneId]);
        while (IsRunning_ && taskQuene->second.empty()) {
            LOG(INFO)<<"pthread_cond_wait, taskQueneId:"<<taskQueneId;
            //pthread_cond_wait()->unlock()->lock()
            pthread_cond_wait(&conditions_[taskQueneId], &mutexs_[taskQueneId]);
        }
        //由于改成多个任务队列，不能使用条件变量,要一直主动去task列表里面取
        if (!IsRunning_) {
            pthread_mutex_unlock(&mutexs_[taskQueneId]);
            break;

        } else if (taskQuene->second.empty()) {
            pthread_mutex_unlock(&mutexs_[taskQueneId]);
            continue;
        }
        assert(!taskQuene->second.empty());
        LOG(INFO)<<"take a task from "<<taskQueneId;
        task = taskQuene->second.front();
        taskQuene->second.pop_front();
        pthread_mutex_unlock(&mutexs_[taskQueneId]);
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


fdemo::mockslave::SlaveHandler* ThreadPool::getMysqlConnect(int taskMapId){
    if(taskMapId >= Maxnum_){
        return NULL;
    }
    return mysqlConnect_[taskMapId];
}

} //namespace common
} //namespace fdemo
