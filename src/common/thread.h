#ifndef FDEMO_COMMON_THREAD_H_
#define FDEMO_COMMON_THREAD_H_
#define DECLARE_CLASS_NAME(Name) static const char *getClassName() { return #Name; }

#include "common/socketinc.h"

namespace fdemo{
namespace common{

class Runable {
public:
    virtual ~Runable() {}
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

class MockTask: public Runable {
public:
    MockTask(const std::string& arg):arg_(arg) {}
    virtual void run();
private:
    const std::string arg_;
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

    static void schedule(Runable* func);

    static void schedule_detach(Runable* func);

private:
    pthread_t tid_;
    Runable *runable_;
};

class ClientHandle: public Runable {
public:
    ClientHandle(int client): socket_client_(client), connected_(true) {
        //SetTimeout(20 * 1000); // 20 seconds
    };
    virtual ~ClientHandle(){
        Close();
    };
    virtual void run();
    void SetTimeout(int milliseconds);
    int ReadMsg(char* data, size_t* size);
    int WriteMsg();
    bool GetRemoteIp(std::string *ip, int* port);
    void Close();

private:
    int socket_client_;
    bool connected_;
    Mutex state_lock_;
};

class SocketSvr {
public:
    SocketSvr() : socket_(-1) {};
    virtual ~SocketSvr(){};
    bool Listen(const std::string host, int port);
    void Close();
    void Run();
    static void AddTotal();
    static int GetTotal();

private:
    int socket_;
    static int total_;
};


} // namespace common
} //namespace common

#endif
