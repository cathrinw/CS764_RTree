CPP=g++
CPPFLAGS=-m32 -Wall

all: rtreeExpRunner

clean: 
	rm -f rtreeExpRunner RTree.o ExpRunner.o *.pdf 

RTree.o: RTree.cpp
	$(CPP) $(CPPFLAGS) -c RTree.cpp

ExpRunner.o: ExpRunner.cpp
	$(CPP) $(CPPFLAGS) -c ExpRunner.cpp

rtreeExpRunner: ExpRunner.o RTree.o
	$(CPP) $(CPPFLAGS) -o rtreeExpRunner RTree.o ExpRunner.o

