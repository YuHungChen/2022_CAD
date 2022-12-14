all : HW2

CXX = g++ -std=c++11 -g
LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3
HEADERS = B_tree.h Treenode.h
SOURCES = Lab2.cpp B_tree.cpp
OBJECTS = Lab2.o B_tree.o


HW2 : main.o Circuit.o
	$(CXX) $(LINKFLAGS) main.o Circuit.o -o HW2.o

main.o : main.cpp
	$(CXX) $(LINKFLAGS) -DCOMPILETIME="\"'date'\"" main.cpp -c
 
Circuit.o : Circuit.cpp Circuit.h Gate.h 
	$(CXX) $(LINKFLAGS) Circuit.cpp -c 
 
clean:
	rm -rf *.o *.gch HW2
