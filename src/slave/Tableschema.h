//Author: demon1991yl@gmail.com

#ifndef FDEMO_SLAVE_TABLESCHEMA_H
#define FDEMO_SLAVE_TABLESCHEMA_H

#include <vector>
#include <string>
#include "slave/bytearray.h"
#include "glog/logging.h"

namespace fdemo{
namespace slave{

class Field {

public:
    Field(const std::string& name): name_(name){}
    virtual ~Field() {}

    const std::string& fieldName() { return name_;}
    virtual std::string valueString(const ByteArray& bytes) = 0;
    virtual std::string valueDefault() = 0;

private:
    std::string name_;
};

class FieldInteger: public Field {
	int length_;
	bool unsigned_;
public:
	explicit FieldInteger(const std::string &name, int length, bool is_unsigned): Field(name), length_(length), unsigned_(is_unsigned) {}

	virtual ~FieldInteger() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return "0"; }
};	//FieldInteger

class FieldTimestamp : public Field {public:explicit FieldTimestamp(const std::string &name): Field(name) {}

	virtual ~FieldTimestamp() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return "1970-01-01 08:00:00" ; }
};

class FieldFloat: public Field {
public:
	explicit FieldFloat(const std::string &name): Field(name) {}

	virtual ~FieldFloat() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return "0.0"; }
};	//FieldFloat

class FieldDouble: public Field {
public:
	explicit FieldDouble(const std::string &name): Field(name) {}

	virtual ~FieldDouble() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return "0.0"; }
};	//FieldDouble

class FieldDatetime: public Field {
	static const int MAX_DATETIME_WIDTH = 19;      /* YYYY-MM-DD HH:MM:SS */
    //only declare
    static int type_;
public:
	explicit FieldDatetime(const std::string &name): Field(name) {}

	virtual ~FieldDatetime() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return "1970-01-01 08:00:00"; }
    static void setColumnMeta(int i) { FieldDatetime::type_ = i; }
};	//FieldDatetime

class FieldDate: public Field {
	static const int MAX_DATE_WIDTH = 10;      /* YYYY-MM-DD */
public:
	explicit FieldDate(const std::string &name): Field(name) {}

	virtual ~FieldDate() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return "1970-01-01"; }
};	//FieldDate

class FieldTime: public Field {
	static const int MAX_TIME_WIDTH = 10;      /* HH:MM:SS */
public:
	explicit FieldTime(const std::string &name): Field(name) {}

	virtual ~FieldTime() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return "00:00:00"; }
};	//FieldTime

class FieldString: public Field {
	int pack_length_;
public:
	explicit FieldString(const std::string &name, int pack_length): Field(name), pack_length_(pack_length) {}

	virtual ~FieldString() {}

	virtual std::string valueString(const ByteArray &b);

	virtual std::string valueDefault() { return ""; }
};	//StringField

class TableSchema {
private:
    std::string dbname_;
    std::string tablename_;
    std::vector<Field*> columns_;
    std::string pk_;
public:
    TableSchema(const std::string& dbname, const std::string& tablename): dbname_(dbname), tablename_(tablename){}
    virtual ~TableSchema();

    const std::string &getTablename() const { return tablename_; }
    const std::string &getDBname() const { return dbname_; }
    std::string getPrikey() { return pk_; }

    bool createField(const char* name, const char* type, const char* max_octet_length);
    Field* getFieldByName(std::string& FieldName);
    Field* getFieldByIndex(int index);
    size_t getFieldCount() const { return columns_.size();}
};

} //namespace slave
} //namespace fdemo
#endif
