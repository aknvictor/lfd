CC = g++
CFLAGS = -pedantic -Wall -g -std=c++0x -I../.. -I/usr/local/include 
LDFLAGS = -L../.. -L/usr/local/lib -lspnav -lX11 -lm controllerInterface.cpp

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

.PHONY: all
all: application

application: controllerApplication.cpp
	$(CC) $(CFLAGS) -DBUILD_AF_UNIX -o servocontroller $< $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f servocontroller
