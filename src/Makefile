CC=gcc
CXX=g++
CXXFLAGS=-c -g -std=c++14
LNFLAGS=-lpanel -lncurses -lsqlite3

EXEC = dice
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LNFLAGS) -o $(EXEC)
 
# To obtain object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
 
# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)

#sqlite3.o:
#	$(CC) -Wall -Werror -Wno-unused sqlite3.c -c

