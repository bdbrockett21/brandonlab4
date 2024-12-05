# Makefile for OS Programming Lab Memory Management Project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 

# Target executable name
TARGET = memory_manager

# Source files
SRCS = address.c

# Object files (generated from source files)
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = lab4.h

# Default target
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compiling source files to object files
%.o: %.c $(HEADERS)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	@echo "Running $(TARGET)"
	./$(TARGET)

# Clean up build artifacts
clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) $(TARGET)

# Rebuild everything from scratch
rebuild: clean all

# Debug target
debug: CFLAGS += -DDEBUG
debug: all

# Valgrind memory check
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Phony targets (targets that don't represent files)
.PHONY: all clean run rebuild debug memcheck