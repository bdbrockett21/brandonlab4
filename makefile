# Makefile for the translate program

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Executable name
EXEC = translate

# Source files
SOURCES = address.c main.c stat.c

# Header files
HEADERS = lab4.h

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(EXEC)

# Linking the executable
$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJECTS)

# Compiling source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJECTS) $(EXEC)

# Phony targets
.PHONY: all clean
