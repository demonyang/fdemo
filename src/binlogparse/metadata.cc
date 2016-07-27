//Author: demon1991yl@gmail.com

#include "binlogparse/metadata.h"


namespace fdemo{
namespace slave{

void MysqlMeta::init(fdemo::utils::XmlConfig& xml) {
    srcMysqlInfo_.name = xml.getNode("src-mysql/name");
    srcMysqlInfo_.host = xml.getNode("src-mysql/ip");
    srcMysqlInfo_.port = xml.getNumber("src-mysql/port");
    srcMysqlInfo_.user = xml.getNode("src-mysql/user");
    srcMysqlInfo_.passwd = xml.getNode("src-mysql/pass");
    srcMysqlInfo_.default_file = xml.getNode("src-mysql/file");
    srcMysqlInfo_.default_offset = xml.getNumber("src-mysql/offset");
    srcMysqlInfo_.server_id = xml.getNumber("src-mysql/serverid");

    poolSize_ = xml.getNumber("pool");
}

} // namespace slave
} // namespace fdemo
