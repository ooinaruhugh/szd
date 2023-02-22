CXX=clang++
CXXFLAGS=-I. -std=c++17
# CXXFLAGS=-I. -g
INCLUDES=
DEPS = util.h zip/zip.h zip/cdr.h zip/eocdr.h zip/LocalHeader.h
SRCS = main.cpp util.cpp zip/zip.cpp zip/cdr.cpp zip/eocdr.cpp zip/LocalHeader.cpp
OBJS = $(SRCS:.cpp=.o)

DEBUG_FORMAT = gdb

MAINNAME = szd

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS   +=-DDEBUG -Og -g$(DEBUG_FORMAT)
	CXXFLAGS +=-DDEBUG -Og -g$(DEBUG_FORMAT)
else
	CFLAGS	 +=-DNDEBUG
	CXXFLAGS +=-DNDEBUG
endif

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -lboost_program_options -o $(MAINNAME) $(OBJS)

# zipinfo: $(OBJS)
# 	$(CXX) -o zipinfo $(OBJS)

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

depend: $(SRCS)
	# makedepend $(INCLUDES) $^
	makedepend $^

clean: 
	$(RM) $(OBJS) $(MAINNAME)

.PHONY: depend clean
