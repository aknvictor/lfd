CC = g++
CFLAGS = -pedantic -Wall -g -std=c++0x -I../.. -I/usr/local/include 
LDFLAGS = -L../.. -L/usr/local/lib -lspnav -lX11 -lm servoController/controllerInterface.cpp predictiveSeqLearning/pslImplementation.cpp predictiveSeqLearning/approxmatch.cpp lfdApplication/appImplementation.cpp cameraInvPerspectiveMonocular/cameraInvPerspectiveMonocularImplementation.cpp

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

.PHONY: all
all: application

application: lfdapplication.cpp
	$(CC) $(CFLAGS) -DBUILD_AF_UNIX -o application $< $(LDFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f application
