CC=g++
CFLAGS=-c -g -Wall -std=c++11 -I./  `pkg-config --cflags opencv`
LDFLAGS= -O3 `pkg-config --libs opencv` -lboost_timer -lboost_system -lboost_thread
CONTROL= control
VISION = vision
GPIO   = gpio
WEB	   = web
SOURCES= test.cpp CameraSource.cpp
OBJECTS=$(SOURCES:.cpp=.o)                                                                           
EXECUTABLE=test	

all:$(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS)  -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clear:
	rm -rf *.o
	


