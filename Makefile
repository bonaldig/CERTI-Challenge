# MAKEFILE

CC=g++

INC_DIR=.

FLAGS=-Wall
FLAGS += -g
FLAGS += -I$(INC_DIR)
FLAGS += -lusb-1.0

TARGET_EXTENSION=.o

all: test
	 
clean:
	rm src/*~ src/*.o

test:
	$(CC) $(FLAGS) lib/DeviceListener.cpp src/main.cpp -o src/test-lib$(TARGET_EXTENSION)

test-libusb:
	$(CC) src/test.cpp -o test$(TARGET_EXTENSION)
