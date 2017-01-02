CC := g++
#CFLAGS := -g3 -O0 -std=c++1z -Wall -c 
CFLAGS := -O3 -std=c++1z -Wall -c
LFLAGS := -o

TARGET := viterbi
SOURCE := $(TARGET).cpp
OBJECTS := $(TARGET).o


all:$(TARGET)

test:$(TARGET)
	./viterbi input_10.txt out

gdb:$(TARGET)
	gdbtui --args  ./viterbi input_10.txt out
#gdbtui -x gdb.txt --args  ./viterbi input_10.txt out

$(TARGET):$(OBJECTS)
	$(CC) $(LFLAGS) $@ $^

$(OBJECTS):$(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
