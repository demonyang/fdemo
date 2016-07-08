//Author: demon1991yl@gmail.com

#include "slave/Tableschema.h"

namespace fdemo{
namespace slave{

TableSchema::~TableSchema() {
    for(size_t i = 0; i<columns_.size();++i) {
        delete columns_[i];
    }
}

bool TableSchema::createField(const char* name, const char* type, const char* max_size) {
    return true;
}

} //namespace slave
} //namespace fdemo
