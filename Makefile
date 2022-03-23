CC=g++
LDFLAGS=-std=c++11 -O3 -lm
SOURCES=src/parser.cpp src/fmBucket.cpp src/fmNet.cpp src/fmNode.cpp src/fmPart.cpp src/main.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=fm
INCLUDES=src/parser.h src/fmDM.h

all: $(SOURCES) bin/$(EXECUTABLE)

bin/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

t0:
	./bin/$(EXECUTABLE) input_pa1/input_0.dat output/output_0.txt
t1:
	./bin/$(EXECUTABLE) input_pa1/input_1.dat output/output_1.txt
t2:
	./bin/$(EXECUTABLE) input_pa1/input_2.dat output/output_2.txt
t3:
	./bin/$(EXECUTABLE) input_pa1/input_3.dat output/output_3.txt
t4:
	./bin/$(EXECUTABLE) input_pa1/input_4.dat output/output_4.txt
t5:
	./bin/$(EXECUTABLE) input_pa1/input_5.dat output/output_5.txt

clean:
	rm -rf *.o bin/$(EXECUTABLE)
