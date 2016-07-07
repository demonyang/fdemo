//Author: demon1991yl@gmail.com

#include <cstdio>
#include "gflags/gflags.h"
#include "utils/utils_cmd.h"
#include "utils/utils_alg.h"
#include "zk/zk_adpter.h"
#include "common/thread.h"
#include "common/threadpool.h"
#include "binlogevent/binlogsync.h"
#include "slave/metadata.h"

DEFINE_string(fdemo_log_prefix, "fdemo", "program's log name");
DEFINE_string(zkServer, "127.0.0.1:2181", "zookeeper address");
DEFINE_string(SvrAddr, "127.0.0.1", "Svr's address");
DEFINE_int32(SvrPort, 1234, "Svr's port");

int main(int argc, char** argv) {
    //set glog's dir. default is <log_dir>
    google::SetVersionString("1.0.0");
    google::SetUsageMessage("Usage: ./fdemo arg...");
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::InitGoogleLogging(argv[0]);
    FLAGS_colorlogtostderr=true;
    FLAGS_log_dir = "/opt/c++/log"; //path of log
    FLAGS_logbufsecs = 0; //日志实时输出
    //google::FlushLogFiles(google::ERROR);
    if (!FLAGS_fdemo_log_prefix.empty()) {
        //set logpath
        fdemo::utils::SetProLog(FLAGS_fdemo_log_prefix);
    }

    for(int i = 0; i<argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    if (FLAGS_zkServer.empty()) {
        LOG(ERROR) << "zkServer can't be null";
        exit(1);
    }
    std::cout<< "logPath:" << FLAGS_fdemo_log_prefix<<std::endl;
    std::cout<< "zk_address:" << FLAGS_zkServer<<std::endl;
    std::cout<< "svr_address" << FLAGS_SvrAddr<<std::endl;
    std::cout<< "svr_port" << FLAGS_SvrPort<<std::endl;

    /*  zookeeper's test
    LOG(INFO)<<"star test of zookeeper";
    fdemo::zk::ZooKeeperAdapter zk_ins;

    int* zk_err  = NULL;
    bool zk_init_ret = zk_ins.Init(FLAGS_zkServer, "/bamboo", 100000, "id1", zk_err);
    if (!zk_init_ret) {
        LOG(ERROR) << "Init zookeeper failed!";
    }
    bool c_ret = zk_ins.CreatePersistentNode("/test1", "first", NULL);
    if (!c_ret) {
        LOG(ERROR) << "CreatePersistentNode failed!";
    }
    LOG(INFO)<<"end zk";
    */

    /* sort test
    int a1[] = {56,32,4,6,123};
    fdemo::utils::SortSet<int> mysort; 
    int len = sizeof(a1)/ sizeof(a1[0]);
    printf("main len: %d\n", len);
    //mysort.MergeSort(a1, 0, 4);
    //mysort.QuickSort(a1, 0 ,4);
    mysort.SelectSort(a1, 4);
    for (int i=0;i<5;i++) {
        printf("num:%d\n", a1[i]);
    }
    */
    

    /*sock test
    fdemo::common::SocketSvr testsvr;
    bool lis_ret = testsvr.Listen(FLAGS_SvrAddr, FLAGS_SvrPort);
    if (!lis_ret) {
        LOG(ERROR)<<"listen error";
        return -1;
    }
    testsvr.Run();
    */
    
    /*ThreadPool test
    fdemo::common::ThreadPool pool(10);
    std::stringstream ss;
    for (int i=0;i<50;i++) {
        ss<<"I am MockTask, num:"<<i;
        pool.AddTask(new fdemo::common::MockTask(ss.str()));
        ss.str("");
    }
    while(1) {
        if(pool.size() == 0) {
            pool.stop();
            LOG(INFO)<<"all task done!";
            return 0;
        }
        sleep(2);
        LOG(INFO)<<"pool size:"<<pool.size();
    }
    */

    fdemo::slave::BinlogInfo bi1 = {"svr1", "218.60.99.62", 6301,"root", "test,.6301","mysql-bin.000001",1234, 1234};
    fdemo::binlogevent::BinlogSync sync(bi1);
    fdemo::common::Thread::schedule_detach(&sync);
    google::ShutdownGoogleLogging();
    return 0;
}
