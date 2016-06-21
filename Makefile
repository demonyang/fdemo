CC = g++
CXXFLAG = -Wall

INCPATH = -I./thirdparty/include
LDPATH = -L./thirdparty/lib -lgflags -lpthread -lglog
OBJECTS = ./fdemon_main.cc

demo: $(OBJECTS)
	$(CC) $(CXXFLAG) -o $@ $^  $(INCPATH) $(LDPATH) 

clean:
	rm -f demo
