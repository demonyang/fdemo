//Author: demon1991yl@gmail.com

#include <iostream>
#include <cstdio>
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(logPath, "/tmp/fdemo/", "program's log path");
DEFINE_string(zkServer, "127.0.0.1:2181", "zookeeper address");

int main(int argc, char** argv) {
    ::google::SetVersionString("1.0.0");
    ::google::SetUsageMessage("Usage: ./fdemo arg...");
    ::google::ParseCommandLineFlags(&argc, &argv, true);

    google::InitGoogleLogging(argv[0]);
    if (!FLAGS_logPath.empty()) {
        //set logpath
        //TODO
    } else {
        // set default logpath
    }

    for(int i = 0; i<argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    std::cout<< "logPath:" << FLAGS_logPath<<std::endl;
    std::cout<< "address:" << FLAGS_zkServer<<std::endl;

    return 0;
}
