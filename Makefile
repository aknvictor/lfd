CC = g++
CFLAGS = -pedantic -Wall -g -I../.. -I/usr/local/include 
LDFLAGS = -L../.. -L/usr/local/lib -lspnav -lX11 -lm controller/controllerInterface.cpp psl/pslImplementation.cpp psl/approxmatch.cpp app/appImplementation.cpp

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

.PHONY: all
all: main

main: app.cpp
	$(CC) $(CFLAGS) -DBUILD_AF_UNIX -o $@ $< $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f main
