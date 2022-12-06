CXX=g++
CXXFLAGS=-I.
INCLUDES=
DEPS = zip/zip.h util.h
SRCS = main.cpp util.cpp zip/zip.cpp
OBJS = $(SRCS:.cpp=.o)

MAINNAME = szd

main: $(OBJS)
	$(CXX) -o $(MAINNAME) $(OBJS)

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