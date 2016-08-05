//Author: demon1991yl@gmail.com

#include "gflags/gflags.h"
#include "utils/utils_cmd.h"
#include "utils/utils_config.h"
#include "common/threadpool.h"
#include "mockslave/binlogsync.h"
#include "binlogparse/metadata.h"
#include "signal.h"

DEFINE_string(fdemo_log_prefix, "binlog_parse", "program's log name");
DEFINE_string(CnfPath, "./config.xml", "config file's path");

static volatile int IsRunning = 1;
static void SignalHandler(int sig){
    exit(0);
}


int main(int argc, char** argv) {
    //set glog's dir. default is <log_dir>
    google::SetVersionString("1.0.0");
    google::SetUsageMessage("Usage: binlog_parse config.xml");
    google::ParseCommandLineFlags(&argc, &argv, true);


    FLAGS_colorlogtostderr=true;
    FLAGS_log_dir = "/opt/c++/log"; //path of log
    FLAGS_logbufsecs = 0; //日志实时输出
    //google::FlushLogFiles(google::ERROR);
    google::InitGoogleLogging(argv[0]);

    if (!FLAGS_fdemo_log_prefix.empty()) {
        //set logpath
        fdemo::utils::SetProLog(FLAGS_fdemo_log_prefix);
    }

    LOG(INFO)<<"fdemo_log_prefix: "<< FLAGS_fdemo_log_prefix<<" ,logPath:"<<FLAGS_log_dir;
    LOG(INFO)<<"config file: "<< FLAGS_CnfPath;
    
    signal(SIGINT, SignalHandler);
    fdemo::utils::XmlConfig cnf;
    cnf.loadFile(FLAGS_CnfPath.c_str());
    fdemo::mockslave::BinlogSync sync(cnf);
    sync.run();
    //after catch ctrl-c signal
    //TODO

    google::ShutdownGoogleLogging();
    return 0;
}
