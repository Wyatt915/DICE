CC=gcc
CXX=g++
CXXFLAGS=-c -std=c++14
LNFLAGS=-lpthread -ldl -lncurses -lsqlite3 

all: dice

dice: main.o editor.o curseswrapper.o sqlite3.o
	$(CXX) main.o editor.o curseswrapper.o sqlite3.o -o dice $(LNFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

editor.o: editor.cpp editor.hpp
	$(CXX) $(CXXFLAGS) editor.cpp

curseswrapper.o: curseswrapper.cpp curseswrapper.hpp
	$(CXX) $(CXXFLAGS) curseswrapper.cpp

sqlite3.o:
	$(CC) -Wall -Werror sqlite3.c -c

clean:
	rm *.o
