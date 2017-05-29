CC=gcc
CXX=g++
CXXFLAGS=-c -std=c++14
LNFLAGS=-lpanel -lncurses -lsqlite3

all: dice

dice: main.o editor.o curseswrapper.o listview.o roller.o
	$(CXX) main.o editor.o curseswrapper.o listview.o roller.o -o dice $(LNFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

listview.o: listview.cpp listview.hpp
	$(CXX) $(CXXFLAGS) listview.cpp

editor.o: editor.cpp editor.hpp
	$(CXX) $(CXXFLAGS) editor.cpp

curseswrapper.o: curseswrapper.cpp curseswrapper.hpp
	$(CXX) $(CXXFLAGS) curseswrapper.cpp

roller.o: roller.cpp roller.hpp
	$(CXX) $(CXXFLAGS) roller.cpp

sqlite3.o:
	$(CC) -Wall -Werror -Wno-unused sqlite3.c -c

clean:
	rm *.o
