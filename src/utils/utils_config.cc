//Author: demon1991yl@gmail.com

#include "utils/utils_config.h"

namespace fdemo{
namespace utils{

XmlConfig::~XmlConfig(){
    delete doc_;
}

void XmlConfig::loadFile(const char* path) {
    doc_ = new TiXmlDocument(path);
    doc_->LoadFile();

    root_ = doc_->RootElement();
}

const char* XmlConfig::getNode(const char* nodePath) {
    std::string p(nodePath);
    char* tmp = NULL;
    const char* name; //= strtok_r((char*)nodePath, "/", &tmp);
    TiXmlElement* e = root_;
    char * buf = (char*)p.c_str();
    while((name = strtok_r(buf, "/", &tmp)) != NULL) {
        e = e->FirstChildElement(name);
        buf = NULL;
    }
    const char* value  = e->GetText();
    return value;

}

uint64_t XmlConfig::getNumber(const char* nodePath) {
    const char * value = getNode(nodePath);
    return (value != NULL ? atoi(value) : 0);
}

} // namespace common
} // namespace fdemo

