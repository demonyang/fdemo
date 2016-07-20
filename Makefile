CXX = g++
CXXFLAG = -std=c++11 -Wl,-rpath,./thirdparty
#CXXFLAG = -Wall -Werror -std=c++11 -Wl,-rpath,./thirdparty
#CXXFLAG = -Wall -Werror -std=c++11 -Wl,-rpath,./thirdparty -ggdb #debug for gdb

INCPATH = -I./thirdparty/include -Isrc/ -I/usr/include/mysql
LDPATH = -L./thirdparty/lib -lgflags -lpthread -lglog -lzookeeper_mt

ZK_SRC = $(wildcard ./src/zk/*.cc)
UTIL_SRC = $(wildcard ./src/utils/*.cc)
COMMON_SRC = $(wildcard ./src/common/*.cc)
SLAVE_SRC = $(wildcard ./src/slave/*.cc)
BINLOG_SRC = $(wildcard ./src/binlogevent/*.cc)
SVR_SRC = ./src/fdemon_main.cc

ZK_OBJ = $(ZK_SRC:.cc=.o)
UTIL_OBJ = $(UTIL_SRC:.cc=.o)
COMMON_OBJ = $(COMMON_SRC:.cc=.o)
SLAVE_OBJ = $(SLAVE_SRC:.cc=.o)
BINLOG_OBJ = $(BINLOG_SRC:.cc=.o)
SVR_OBJ = $(SVR_SRC:.cc=.o)
ALL_OBJ = $(SVR_OBJ) $(ZK_OBJ) $(UTIL_OBJ) $(COMMON_OBJ) $(BINLOG_OBJ) $(SLAVE_OBJ) 

all: binlog_parse

binlog_parse: $(ALL_OBJ)
	$(CXX) $(CXXFLAG) -o $@ $^  $(LDPATH) ./thirdparty/libmysqlclient_r.so.15

.cc.o:
	$(CXX) $(CXXFLAG) -c -o $@ $< $(INCPATH)
clean:
	rm -f binlog_parse src/*/*.o src/*.o
