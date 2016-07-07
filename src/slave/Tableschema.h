//Author: demon1991yl@gmail.com

#ifndef FDEMO_SLAVE_TABLESCHEMA_H
#define FDEMO_SLAVE_TABLESCHEMA_H

#include <vector>
#include <string>

namespace fdemo{
namespace slave{

class Field {

public:
    Field(const std::string& name): name_(name){}
    virtual ~Field();

    const std::string& findName() { return name_;}
private:
    std::string name_;
};

class TableSchema {
private:
    std::string dbname_;
    std::string tablename_;
    std::vector<Field*> columns_;
public:
    TableSchema(const std::string& dbname, const std::string& tablename): dbname_(dbname), tablename_(tablename){}
    virtual ~TableSchema();

    const std::string &getTablename() const { return tablename_; }
    const std::string &getDBname() const { return dbname_; }

    bool createField(const char* name, const char* type, const char* max_size);
    Field* getFieldByName(std::string& FieldName);
    Field* getFieldByIndex(int index);
    size_t getFieldCount() const { return columns_.size();}
};

} //namespace slave
} //namespace fdemo
#endif
