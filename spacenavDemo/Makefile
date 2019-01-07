CC = g++
CFLAGS = -pedantic -Wall -g -std=c++0x -I../.. -I/usr/local/include 
LDFLAGS = -L../.. -L/usr/local/lib -lspnav -lX11 -lm spacenavInterface.cpp


.PHONY: all
all: application

application: spacenavDemoApplication.cpp
	$(CC) $(CFLAGS) -DBUILD_AF_UNIX -o spacenavdemo $< $(LDFLAGS) 

.PHONY: clean
clean:
	rm -f spacenavdemo
