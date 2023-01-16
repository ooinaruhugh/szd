CXX=g++
CXXFLAGS=-I. -lboost_program_options
# CXXFLAGS=-I. -g
INCLUDES=
DEPS = util.h zip/zip.h zip/cdr.h zip/eocdr.h zip/LocalHeader.h
SRCS = main.cpp util.cpp zip/zip.cpp zip/cdr.cpp zip/eocdr.cpp zip/LocalHeader.cpp
OBJS = $(SRCS:.cpp=.o)

MAINNAME = szd

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS   +=-DDEBUG -Og
	CXXFLAGS +=-DDEBUG -Og
else
	CFLAGS	 +=-DNDEBUG
	CXXFLAGS +=-DNDEBUG
endif

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(MAINNAME) $(OBJS)

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
