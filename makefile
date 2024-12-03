# Compiler and flags
CC = gcc
CFLAGS = -O3 -mavx2

# Target executable
TARGET = db5242

# Source file
SRC = db5242.c

# Default target
all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean target
clean:
	rm -f $(TARGET)