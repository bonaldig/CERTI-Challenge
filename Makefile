# MAKEFILE

CC=gcc

INC_DIR=.

FLAGS=-Wall
FLAGS += -g
FLAGS += -I$(INC_DIR)

TARGET_EXTENSION=.o

all: clean test
	 
clean:
	rm src/*~ src/*.o

test:
	$(CC) $(FLAGS) lib/DeviceListener.cpp src/main.cpp -o src/test-lib$(TARGET_EXTENSION)
