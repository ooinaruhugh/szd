CC=clang
CXX=clang++
override CFLAGS+=-I. -std=c17
override CXXFLAGS+=-I. -std=c++20
INCLUDES=
DEPS = util.h memmem.h zip/zip.h zip/cdr.h zip/eocdr.h zip/LocalHeader.h 
SRCS = main.cpp util.cpp memmem.c zip/zip.cpp zip/cdr.cpp zip/eocdr.cpp zip/LocalHeader.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

PREFIX=/usr/local

DEBUG_FORMAT = gdb

MAINNAME = szd
TARGETS = $(MAINNAME)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	override CFLAGS   +=-DDEBUG -Og -g$(DEBUG_FORMAT)
	override CXXFLAGS +=-DDEBUG -Og -g$(DEBUG_FORMAT)
else
	override CFLAGS   +=-DNDEBUG
	override CXXFLAGS +=-DNDEBUG
endif

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -lboost_program_options -o $(MAINNAME) $(OBJS)

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

.PHONY: depend clean install uninstall