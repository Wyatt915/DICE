CC=gcc
CXX=g++
CXXFLAGS=-c -std=c++14
LNFLAGS=-lncurses
SQLITEFLAGS=-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION

all: dice

dice: main.o editor.o buffer.o
	$(CXX) main.o editor.o buffer.o -o dice $(LNFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

editor.o: editor.cpp editor.hpp
	$(CXX) $(CXXFLAGS) editor.cpp

buffer.o: buffer.cpp buffer.hpp
	$(CXX) $(CXXFLAGS) buffer.cpp

clean:
	rm *.o
