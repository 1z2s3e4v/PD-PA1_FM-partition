CC=g++
LDFLAGS=-std=c++11 -O3 -lm
SOURCES=src/parser.cpp src/fmBucket.cpp src/fmNet.cpp src/fmNode.cpp src/fmPart.cpp src/main.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=fm
INCLUDES=src/parser.h src/fmDM.h

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
