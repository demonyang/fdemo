//Author: demon1991yl@gmail.com

#include "slave/Tableschema.h"
#include <mysql/my_global.h>
#include <slave/logevent.h>

namespace fdemo{
namespace slave{

//FieldInteger
std::string FieldInteger::valueString(const ByteArray &b) {
	char buf[32];
	int len;

	union {
		int64_t i;
		uint64_t ui;
	} value;

	if (length_ == 1) {
		unsigned_ ? value.ui = b.getFixed8() : value.i = (int8_t) b.getFixed8();
	} else if (length_ == 2) {
		unsigned_ ? value.ui = b.getFixed16() : value.i = (int16_t) b.getFixed16();
	} else if (length_ == 3) {
		const char *ptr = b.get(3);
		unsigned_ ? value.ui = uint3korr(ptr) : value.i = sint3korr(ptr);
	} else if (length_ == 4) {
		unsigned_ ? value.ui = b.getFixed32() : value.i = (int32_t) b.getFixed32();
	} else if (length_ == 8) {
		unsigned_ ? value.ui = b.getFixed64() : value.i = (int64_t) b.getFixed64();
	}

	if (length_ == 8 && unsigned_) {
#if __WORDSIZE == 64
		len = snprintf(buf, sizeof(buf), "%lu", value.ui);
#else
		len = snprintf(buf, sizeof(buf), "%llu", value.ui);
#endif
	} else {
#if __WORDSIZE == 64
		len = snprintf(buf, sizeof(buf), "%ld", value.i);
#else
		len = snprintf(buf, sizeof(buf), "%lld", value.i);
#endif
	}
	return std::string(buf, len);
}

//FieldString
std::string FieldString::valueString(const ByteArray &b) {
	size_t len = 0;
	std::string value;

	if (pack_length_ == 1) {
		len = (size_t) (*((uint8*)b.get(1)));
	} else if (pack_length_ == 2) {
		len = (size_t) b.getFixed16();
	} else if (pack_length_ == 3) {
		len = (size_t) b.getFixed24();
	} else if (pack_length_ == 4) {
		len = (size_t) b.getFixed32();
	}
	value.assign(b.get(len), len);
	return value;
}


//FieldTimestamp
std::string FieldTimestamp::valueString(const ByteArray &b) {
	time_t i = (int32_t) b.getFixed32();
	struct tm* t;                                    
	struct tm tr;                                    
	t = localtime_r(&i, &tr);                         
		                                    
	char buf[32];                                    
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t); 
	return buf ;
}

// base on mysql5.6.25 sql/log_event.cc::log_event_print_value
std::string FieldDatetime::valueString(const ByteArray &b) {
	char buf[MAX_DATETIME_WIDTH + 1];
	int64 value = b.getFixed40_little() - DATETIMEF_INT_OFS;
    LOG(INFO)<<"datetime value:"<<value<<" ,fieldTypeDecimal:"<<fieldTypeTiny;
	if (value == 0) {
		return valueDefault();
	}
    if (value <0) {
        value = -value;
    }
    longlong ymd = value >> 17;
    longlong ym = ymd >> 5;
    longlong hms = value % (1<<17);
    snprintf(buf, sizeof(buf), "%04lld-%02lld-%02lld %02lld:%02lld:%02lld", ym / 13, ym % 13, ymd % (1 << 5), (hms >> 12), (hms >> 6) % (1 << 6), hms % (1 << 6));
    int frac = 0;
    switch (type_) {
        case 0:
        default:
            break;
        case 1:
        case 2:
            frac = b.getFixed8() * 10000;
            break;
        case 3:
        case 4:
            frac = b.getFixed16_little() * 100;
            break;
        case 5:
        case 6:
            frac = b.getFixed24_little();
            break;
    }
    LOG(INFO)<<"frac:"<<frac;
    return std::string(buf);
}

//定义并初始化
int FieldDatetime::type_ = 0;

/*
//FieldDatetime
std::string FieldDatetime::valueString(const ByteArray &b) {
	char buf[MAX_DATETIME_WIDTH + 1];
	long part1,part2;
	char *pos;
	int part3;

	//uint64_t value = b.getFixed64();
	uint64_t value = b.getFixed40();
    LOG(INFO)<<"datetime value:"<<value;
	if (value == 0) {
		return valueDefault();
	}
       
	//Avoid problem with slow longlong arithmetic and sprintf
	part1=(long) (value / 1000000ULL);
	//part2=(long) (value - part1*1000000ULL);
	part2=(long) (value % 1000000ULL);

	pos=(char*) buf + MAX_DATETIME_WIDTH;
	*pos--=0;
	*pos--= (char) ('0'+(char) (part2%10)); part2/=10;
	*pos--= (char) ('0'+(char) (part2%10)); part3= (int) (part2 / 10);
	*pos--= ':';
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= ':';
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) part3);
	*pos--= ' ';
	*pos--= (char) ('0'+(char) (part1%10)); part1/=10;
	*pos--= (char) ('0'+(char) (part1%10)); part1/=10;
	*pos--= '-';
	*pos--= (char) ('0'+(char) (part1%10)); part1/=10;
	*pos--= (char) ('0'+(char) (part1%10)); part3= (int) (part1/10);
	*pos--= '-';
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos--= (char) ('0'+(char) (part3%10)); part3/=10;
	*pos=(char) ('0'+(char) part3);
	return std::string(buf);
}*/

//FieldDate
std::string FieldDate::valueString(const ByteArray &b) {
	char buf[MAX_DATE_WIDTH + 1];
	uint32_t tmp = b.getFixed24();
	if (tmp == 0) {
		return valueDefault();
	}

	int part;
	char *pos= buf + MAX_DATE_WIDTH;

	/* Open coded to get more speed */
	*pos--=0;                                     // End NULL
	part=(int) (tmp & 31);
	*pos--= (char) ('0'+part%10);
	*pos--= (char) ('0'+part/10);
	*pos--= '-';
	part=(int) (tmp >> 5 & 15);
	*pos--= (char) ('0'+part%10);
	*pos--= (char) ('0'+part/10);
	*pos--= '-';
	part=(int) (tmp >> 9);
	*pos--= (char) ('0'+part%10); part/=10;
	*pos--= (char) ('0'+part%10); part/=10;
	*pos--= (char) ('0'+part%10); part/=10;
	*pos=   (char) ('0'+part);
	return std::string(buf);
}

std::string FieldTime::valueString(const ByteArray &b) {
	char buf[MAX_TIME_WIDTH + 1];
    	uint32_t tmp = b.getFixed24();

    	uint32_t hour= (uint) (tmp/10000);
	uint32_t minute= (uint) (tmp/100 % 100);
    	uint32_t second= (uint) (tmp % 100);

	snprintf(buf, sizeof(buf), "%u:%u:%u", hour, minute, second);
	return std::string(buf);
}

//FieldFloat
std::string FieldFloat::valueString(const ByteArray &b) {
	char buf[32];

	float f;
	const char *ptr = b.get(4);
	float4get(f, ptr);
	snprintf(buf, sizeof(buf), "%f", f);
	return std::string(buf);
}

//FieldDouble
std::string FieldDouble::valueString(const ByteArray &b) {
	char buf[64];

	double d;
	const char *ptr = b.get(8);
	doubleget(d, ptr);
	snprintf(buf, sizeof(buf), "%f", d);
	return std::string(buf);
}

TableSchema::~TableSchema() {
    for(size_t i = 0; i<columns_.size();++i) {
        delete columns_[i];
    }
}

//TableSchema
bool TableSchema::createField(const char *name, const char *type, const char *max_octet_length) {
	Field *field = NULL;

	if (strstr(type, "int") != NULL) {
		bool is_unsigned = (strstr(type, "unsigned") == NULL ? false : true);

		if (strstr(type, "tinyint") == type) {
			field = new FieldInteger(name, 1, is_unsigned);

		} else if (strstr(type, "smallint") == type) {
			field = new FieldInteger(name, 2, is_unsigned);

		} else if (strstr(type, "mediumint") == type) {
			field = new FieldInteger(name, 3, is_unsigned);

		} else if (strstr(type, "int") == type) {
			field = new FieldInteger(name, 4, is_unsigned);

		} else if (strstr(type, "bigint") == type) {
			field = new FieldInteger(name, 8, is_unsigned);

		} else {
			assert(false);
		}
	} else if (strstr(type, "varchar") == type || strstr(type, "char") == type) {
		int pack_length;
		atoi(max_octet_length) < 256 ? pack_length = 1 : pack_length = 2;
		field = new FieldString(name, pack_length);

	} else if (strstr(type, "varbinary") == type || strstr(type, "binary") == type) {
		int pack_length;
		atoi(max_octet_length) < 256 ? pack_length = 1 : pack_length = 2;
		field = new FieldString(name, pack_length);

	} else if (strstr(type, "text") != NULL) {
     		int pack_length = 0;
		if (strcmp(type, "tinytext") == 0) {
			pack_length = 1;
		} else if (strcmp(type, "text") == 0) {
			pack_length = 2;
		} else if (strcmp(type, "mediumtext") == 0) {
			pack_length = 3;
		} else if (strcmp(type, "longtext") == 0) {
			pack_length = 4;
		} else {
			assert(false);
		}
		field = new FieldString(name, pack_length);

	} else if (strstr(type, "blob") != NULL) {
		int pack_length = 0;
		if (strcmp(type, "tinyblob") == 0) {
			pack_length = 1;
		} else if (strcmp(type, "blob") == 0) {
			pack_length = 2;
		} else if (strcmp(type, "mediumblob") == 0) {
			pack_length = 3;
		} else if (strcmp(type, "longblob") == 0) {
			pack_length = 4;
		} else {
			assert(false);
		}
		field = new FieldString(name, pack_length);

	} else if (strcmp(type, "time") == 0) {
		field = new FieldTime(name);

	} else if (strcmp(type, "date") == 0) {
		field = new FieldDate(name);

    //depending on mysql version
	} else if (strcmp(type, "datetime") == 0) {
		field = new FieldDatetime(name);

	} else if (strncmp(type, "float",5) == 0) {
		field = new FieldFloat(name);

	} else if (strcmp(type, "double") == 0) {
		field = new FieldDouble(name);
	} else if (strcmp(type, "timestamp") == 0 ) {
		field = new FieldTimestamp(name) ;
	}

	if (field == NULL) {
		LOG(ERROR)<<"column "<<name<<" type not support: "<<type;
		return false;
	}

	columns_.push_back(field);
	return true;
}

Field* TableSchema::getFieldByName(std::string& FieldName) {
    for(size_t i = 0; i<columns_.size();i++) {
        if(columns_[i]->fieldName() == FieldName) {
            return columns_[i];
        }
    }
    return NULL;
}

Field* TableSchema::getFieldByIndex(int index) {
    if(index < 0 || index >= (int)columns_.size()) {
        return NULL;
    }
    return columns_[index];
}

} //namespace slave
} //namespace fdemo
