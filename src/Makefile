CC=gcc
CXX=g++
CXXFLAGS=-c -g -std=c++14
LNFLAGS=-lpanel -lncurses -lpthread

EXEC = dice
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o) sqlite3.o

# Main target
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LNFLAGS) -o $(EXEC)
 
sqlite3.o: sqlite3.c
	$(CC) -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -Wall -Werror -Wno-unused sqlite3.c -c -Os

# To obtain object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(OBJECTS)

