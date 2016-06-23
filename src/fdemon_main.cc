//Author: demon1991yl@gmail.com

#include <iostream>
#include <cstdio>
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "utils/utils_cmd.h"
#include "zk/zk_adpter.h"

DEFINE_string(fdemo_log_prefix, "fdemo", "program's log name");
DEFINE_string(zkServer, "127.0.0.1:2181", "zookeeper address");

int main(int argc, char** argv) {
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

    return 0;
}
