CC=gcc
CXX=g++
CXXFLAGS=-c -std=c++14
LNFLAGS=-lncurses -lsqlite3 
SQLITEFLAGS=-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION

all: dice

dice: main.o editor.o curseswrapper.o
	$(CXX) main.o editor.o curseswrapper.o -o dice $(LNFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

editor.o: editor.cpp editor.hpp
	$(CXX) $(CXXFLAGS) editor.cpp

curseswrapper.o: curseswrapper.cpp curseswrapper.hpp
	$(CXX) $(CXXFLAGS) curseswrapper.cpp

clean:
	rm *.o
