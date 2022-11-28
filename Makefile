CXX=g++
CXXFLAGS=-I.
DEPS = zip.h util.h

main: zip.o main.o
	$(CXX) -o szd zip.o main.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	