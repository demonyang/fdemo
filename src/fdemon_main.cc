//Author: demon1991yl@gmail.com

#include "gflags/gflags.h"
#include "utils/utils_cmd.h"
#include "utils/utils_config.h"
#include "common/threadpool.h"
#include "binlogevent/binlogsync.h"
#include "slave/metadata.h"

DEFINE_string(fdemo_log_prefix, "binlog_parse", "program's log name");
DEFINE_string(CnfPath, "./config.xml", "config file's path");

int main(int argc, char** argv) {
    //set glog's dir. default is <log_dir>
    google::SetVersionString("1.0.0");
    google::SetUsageMessage("Usage: binlog_parse config.xml");
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

    LOG(INFO)<<"fdemo_log_prefix: "<< FLAGS_fdemo_log_prefix<<" ,logPath:"<<FLAGS_log_dir;
    LOG(INFO)<<"config file: "<< FLAGS_CnfPath;
    
    fdemo::utils::XmlConfig cnf;
    cnf.loadFile(FLAGS_CnfPath.c_str());
    fdemo::binlogevent::BinlogSync sync(cnf);
    sync.run();

    google::ShutdownGoogleLogging();
    return 0;
}
