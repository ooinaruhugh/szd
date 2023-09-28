CC=clang
CXX=clang++

override CFLAGS+=-std=c17
override CXXFLAGS+=-std=c++20
LDLIBS=-lboost_program_options 
INCLUDES=-Isrc -Ithird_party

override CPPFLAGS:=$(INCLUDES) $(CPPFLAGS)

DEBUG?=1
DEBUG_FORMAT=gdb
ifeq ($(DEBUG), 1)
override CFLAGS   +=-DDEBUG -Og -g$(DEBUG_FORMAT)
override CXXFLAGS +=-DDEBUG -Og -g$(DEBUG_FORMAT)
else
override CFLAGS   +=-DNDEBUG
override CXXFLAGS +=-DNDEBUG
endif

PREFIX=/usr/local

TARGETS = $(MAINNAME)
MAINNAME = szd

DEPS = src/util.h src/memmem.h src/zip/zip.h src/zip/cdr.h src/zip/eocdr.h src/zip/LocalHeader.h 
SRCS = src/main.cpp src/util.cpp src/memmem.c src/zip/zip.cpp src/zip/cdr.cpp src/zip/eocdr.cpp src/zip/LocalHeader.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

all: $(TARGETS)

main: $(MAINNAME)

szd: $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LOADLIBES) $(LDLIBS)

# zipinfo: $(OBJS)
# 	$(CXX) -o zipinfo $(OBJS)

# %.o: %.cpp $(DEPS)
# 	$(CXX) -c -o $@ $< $(CXXFLAGS)

# %.o: %.c $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	$(RM) $(OBJS) $(MAINNAME)

install:
	@mkdir -p $(PREFIX)/bin
	install $(TARGETS) $(PREFIX)/bin

uninstall:
	rm $(TARGETS:%=$(PREFIX)/bin/%)

.PHONY: main depend clean install uninstall