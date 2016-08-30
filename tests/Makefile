CXX = g++
CXXFLAG = -std=c++11 -Wl,-rpath,../thirdparty
#CXXFLAG = -Wall -Werror -std=c++11 -Wl,-rpath,./thirdparty
#CXXFLAG = -Wall -Werror -std=c++11 -Wl,-rpath,./thirdparty -ggdb #debug for gdb

INCPATH = -I../thirdparty/include -I../src/ -I/usr/include/mysql
LDPATH = -L../thirdparty/lib -lgflags -lpthread -lglog -lzookeeper_mt -ltinyxml

ZK_SRC = $(wildcard ../src/zk/*.cc)
UTIL_SRC = $(wildcard ../src/utils/*.cc)
COMMON_SRC = $(wildcard ../src/common/*.cc)
SVR_SRC = ./alg_test.cc

ZK_OBJ = $(ZK_SRC:.cc=.o)
UTIL_OBJ = $(UTIL_SRC:.cc=.o)
COMMON_OBJ = $(COMMON_SRC:.cc=.o)
SVR_OBJ = $(SVR_SRC:.cc=.o)
ALL_OBJ = $(SVR_OBJ) $(ZK_OBJ) $(UTIL_OBJ) $(COMMON_OBJ)

all: alg_test

alg_test: $(ALL_OBJ)
	$(CXX) $(CXXFLAG) -o $@ $^  $(LDPATH)

.cc.o:
	$(CXX) $(CXXFLAG) -c -o $@ $< $(INCPATH)
clean:
	rm -f alg_test ../src/zk/*.o ./*.o ../src/utils/*.o ../src/common/*.o
