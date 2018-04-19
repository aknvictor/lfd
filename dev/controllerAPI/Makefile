CC = g++
CFLAGS = -pedantic -Wall -g -I../.. -I/usr/local/include 
LDFLAGS = -L../.. -L/usr/local/lib -lspnav -lX11 controllerInterface.cpp appImplementation.cpp -lm

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

.PHONY: all
all: app

app: app.cpp
	$(CC) $(CFLAGS) -DBUILD_AF_UNIX -o $@ $< $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f app
