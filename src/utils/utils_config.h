//Author: demon1991yl@gmail.com
#ifndef FDEMO_UTILS_UTILS_CONFIG_H_
#define FDEMO_UTILS_UTILS_CONFIG_H_

#include "tinyxml/tinyxml.h"
#include <string>
#include <string.h>

namespace fdemo{
namespace utils{

class XmlConfig{
public:
    XmlConfig() {}
    ~XmlConfig();

    void loadFile(const char* path);
    
    const char* getNode(const char* nodePath);
    uint64_t getNumber(const char* nodePath);

private:
    TiXmlElement* root_;
    TiXmlDocument* doc_;
};

} // namespace utils
} // namespace fdemo

#endif
