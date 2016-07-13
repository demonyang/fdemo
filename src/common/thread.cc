// Author:demon1991yl@gmail.com

#include "thread.h"
#include <exception>

namespace fdemo {
namespace common {

void FuncWrap::run() {
    (*func_)();    
}

void MockTask::run() {
    sleep(5);
    std::cout<<"task arg:"<< arg_<<std::endl;
}

void Thread::schedule(Runable* func) {
    Thread * thread =  new Thread(func);
    bool ret = thread->join();
    if (!ret) {
        LOG(ERROR)<<"schedule fail";
    }
    delete thread;
    }

void Thread::schedule_detach(Runable* func) {
    Thread* thread = new Thread(func);
    thread->start();
    thread->detach();
    delete thread;
}

bool Thread::join() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&tid_, &attr, &Thread::proc, runable_);
    void *status;
    int ret = pthread_join(tid_, &status);
    if(ret != 0) {
        LOG(ERROR)<< "pthread_join fail, ret code";
        return false;
    } else {
        LOG(INFO)<< "pthread_join ok";
    }

    pthread_attr_destroy(&attr);
    return true;
    
}

bool SocketSvr::Listen(const std::string host, int port) {
    try
    {
        struct sockaddr_in addr;
        if ((socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
            LOG(ERROR)<<"can't create socket";
            return false;
        }

        int flag = 1;
        setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_port = htons(port);

        if (bind(socket_, (struct sockaddr*) &addr, sizeof(addr)) != 0) {
            LOG(ERROR)<<"bind error";
            Close();
            return false;
        }

        if (listen(socket_, 1024) != 0) {
            LOG(ERROR)<<"listen error";
            Close();
            return false;
        } 
    }
    catch(std::exception& e)
    {
        LOG(ERROR)<<"Listen Exception: "<< e.what();
    }
    return true;
}

void SocketSvr::Close() {
    if (socket_ > 0) {
        close(socket_);
        socket_ = -1;
    }
}

void SocketSvr::Run() {
    int client;
    struct sockaddr_in addr;

    while(1) {
        socklen_t len = sizeof(addr);
        if ((client = accept(socket_, (struct sockaddr*) &addr, &len)) < 0) {
            LOG(ERROR) <<"accept error, but continue";
            continue;
        }
        //start a thread to do 
        LOG(INFO)<<"start schedule_detach job";
        Thread::schedule_detach(new ClientHandle(client));
    }
}

void SocketSvr::AddTotal() {
    total_++;
}

int SocketSvr::GetTotal() {
    return total_;
}

void ClientHandle::run() {
    while(1){
        if (connected_) {
            //check EOF
            //TODO
            std::string ip;
            int port;
            if (!GetRemoteIp(&ip, &port)) {
                LOG(ERROR)<< "GetRemoteIp error";
            }
            //LOG(INFO)<<"start ClientHandle::run(),ip:"<<ip<<"port:"<< port;
            int err;
            char tmp_buf[8192]; //8k
            size_t max_len = 2048;

            if ((err = ReadMsg(tmp_buf, &max_len)) != 0) {
                LOG(ERROR)<< "ReadMsg() errno";
                break;
            }
            LOG(INFO)<<"recv data:"<< tmp_buf;
            memset(tmp_buf, 0, sizeof(tmp_buf));
        } else {
            LOG(ERROR)<< "Not connected";
            break;
        }
    }
    MutexLock mutex(&state_lock_);
    SocketSvr::AddTotal();
    LOG(INFO)<<"total:"<<SocketSvr::GetTotal();
    Close();
}

void ClientHandle::SetTimeout(int milliseconds) {
    struct timeval tv;
    tv.tv_sec = milliseconds / 1000;
    tv.tv_usec = milliseconds % 1000 * 1000;

    setsockopt(socket_client_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(socket_client_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

int ClientHandle::ReadMsg(char* data, size_t* size) {
    int n = recv(socket_client_, data, *size, 0);
    if (n < 0) {
        LOG(ERROR)<<"recv err";
        return -errno;
    } else if (n == 0) {
        return -EPIPE;
    } else {
        *size = n;
        return 0;
    }
}

int ClientHandle::WriteMsg() {
    return 0;
}

bool ClientHandle::GetRemoteIp(std::string* ip, int* port) {
    if (socket_client_ < 0) {
        return false;
    } 

    char ip_buf[16];
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getpeername(socket_client_, (struct sockaddr *) &addr, &len) != 0 ) {
        return -errno;
    }
    
    unsigned char* ptr = (unsigned char*)&addr.sin_addr;
    snprintf(ip_buf, sizeof(ip_buf), "%d.%d.%d.%d", ptr[0], ptr[1], ptr[2], ptr[3]);

    *ip = ip_buf;
    *port = ntohs(addr.sin_port);
    return true;
}

void ClientHandle::Close() {
    if (socket_client_ >= 0) {
        close(socket_client_);
        socket_client_ = -1;
        connected_ = false;
    }
}

int SocketSvr::total_ = 0; //静态变量，不能在头文件中声明定义

} // namespace common
} // namespace fdemo
