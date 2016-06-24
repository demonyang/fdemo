//Author: demon1991yl@gmail.com

#include <iostream>
#include <cstdio>
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "utils/utils_cmd.h"
#include "utils/utils_alg.h"
#include "zk/zk_adpter.h"

DEFINE_string(fdemo_log_prefix, "fdemo", "program's log name");
DEFINE_string(zkServer, "127.0.0.1:2181", "zookeeper address");

int main(int argc, char** argv) {
    //set glog's dir. default is <log_dir>
    FLAGS_log_dir = "/opt/c++/log";
    ::google::SetVersionString("1.0.0");
    ::google::SetUsageMessage("Usage: ./fdemo arg...");
    ::google::ParseCommandLineFlags(&argc, &argv, true);

    google::InitGoogleLogging(argv[0]);
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
    std::cout<< "address:" << FLAGS_zkServer<<std::endl;

    //LOG(INFO)<<"star test of zookeeper";
    //fdemo::zk::ZooKeeperAdapter zk_ins;

    //int* zk_err  = NULL;
    //bool zk_init_ret = zk_ins.Init(FLAGS_zkServer, "/bamboo", 100000, "id1", zk_err);
    //if (!zk_init_ret) {
    //    LOG(ERROR) << "Init zookeeper failed!";
    //}
    //bool c_ret = zk_ins.CreatePersistentNode("/test1", "first", NULL);
    //if (!c_ret) {
    //    LOG(ERROR) << "CreatePersistentNode failed!";
    //}
    int a1[] = {56,32,4,6,123};
    fdemo::utils::SortSet<int> mysort; 
    int len = sizeof(a1)/ sizeof(a1[0]);
    printf("main len: %d\n", len);
    mysort.InsertSort(a1, len);
    for (int i=0;i<5;i++) {
        printf("num:%d\n", a1[i]);
    }

    return 0;
}
