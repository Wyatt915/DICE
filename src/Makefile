CC=gcc
CXX=g++
CXXFLAGS=-std=c++14
LNFLAGS=-lncurses
SQLITEFLAGS=-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION

all: dice

dice: main.cpp sqlite.o editor.o buffer.o
	$(CXX) $(CXXFLAGS) $(LNFLAGS) main.cpp editor.o buffer.o sqlite.o -o dice

nosql: main.cpp editor.o buffer.o
	$(CXX) $(CXXFLAGS) $(LNFLAGS) main.cpp editor.o buffer.o -o dice

sqlite.o: sqlite3.c sqlite3.h
	$(CC) $(LNFLAGS) $(SQLITEFLAGS) -ldl -I sqlite3.c

editor.o: editor.cpp editor.hpp
	$(CXX) $(CXXFLAGS) $(LNFLAGS) -c editor.cpp

buffer.o: buffer.cpp buffer.hpp
	$(CXX) $(CXXFLAGS) -c buffer.cpp

clean:
	rm *.o
