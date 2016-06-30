
#include "utils/utils_cmd.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DECLARE_string(log_dir);

namespace fdemo {
namespace utils{

void SetProLog(const std::string & program_name) {
    std::string log_name = "fdemo";
    if (!program_name.empty()) {
        log_name = program_name;
    }

    std::string  log_filename = FLAGS_log_dir + "/" + log_name + ".INFO";
    std::string  wf_filename = FLAGS_log_dir + "/" + log_name + ".WARN";
    std::string  ef_filename = FLAGS_log_dir + "/" + log_name + ".ERR";

    ::google::SetLogDestination(google::INFO, log_filename.c_str());
    ::google::SetLogDestination(google::WARNING, wf_filename.c_str());
    ::google::SetLogDestination(google::ERROR, ef_filename.c_str());
    ::google::SetLogDestination(google::FATAL, "");

    ::google::SetLogSymlink(google::INFO, log_name.c_str());
    ::google::SetLogSymlink(google::WARNING, log_name.c_str());
    ::google::SetLogSymlink(google::ERROR, log_name.c_str());
    ::google::SetLogSymlink(google::FATAL, "");
}

} //namespace utils
} //namespace fdemo
