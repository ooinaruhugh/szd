CC=clang
CXX=clang++

override CFLAGS+=-I. -std=c17
override CXXFLAGS+=-I. -std=c++20
LDLIBS=-lboost_program_options 
INCLUDES=

DEBUG ?= 1
DEBUG_FORMAT = gdb
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

DEPS = util.h memmem.h zip/zip.h zip/cdr.h zip/eocdr.h zip/LocalHeader.h 
SRCS = main.cpp util.cpp memmem.c zip/zip.cpp zip/cdr.cpp zip/eocdr.cpp zip/LocalHeader.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

main: $(MAINNAME)

szd: $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(OBJS) $(LOADLIBES) $(LDLIBS)

# zipinfo: $(OBJS)
# 	$(CXX) -o zipinfo $(OBJS)

# %.o: %.cpp $(DEPS)
# 	$(CXX) -c -o $@ $< $(CXXFLAGS)

# %.o: %.c $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

depend: $(SRCS)
	# makedepend $(INCLUDES) $^
	makedepend $^

clean: 
	$(RM) $(OBJS) $(MAINNAME)

install:
	@mkdir -p $(PREFIX)/bin
	install $(TARGETS) $(PREFIX)/bin

uninstall:
	rm $(TARGETS:%=$(PREFIX)/bin/%)

.PHONY: main depend clean install uninstall