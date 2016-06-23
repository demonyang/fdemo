CXX = g++
CXXFLAG = -Wall -Werror -std=c++11

INCPATH = -I./thirdparty/include -I./src
LDPATH = -L./thirdparty/lib -lgflags -lpthread -lglog -lzookeeper_mt

ZK_SRC = $(wildcard ./src/zk/*.cc)
UTIL_SRC = $(wildcard ./src/utils/*.cc)
SVR_SRC = ./src/fdemon_main.cc

ZK_OBJ = $(ZK_SRC:.cc=.o)
UTIL_OBJ = $(UTIL_SRC:.cc=.o)
SVR_OBJ = $(SVR_SRC:.cc=.o)
ALL_OBJ = $(SVR_OBJ) $(ZK_OBJ) $(UTIL_OBJ)

all: demo

demo: $(ALL_OBJ)
	$(CXX) $(CXXFLAG) -o $@ $^  $(INCPATH) $(LDPATH) 

.cc.o:
	$(CXX) $(CXXFLAG) -c -o $@ $< $(INCPATH) $(LDPATH) 
clean:
	rm -f demo src/*/*.o src/*.o
