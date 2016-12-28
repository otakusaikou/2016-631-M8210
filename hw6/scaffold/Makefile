# Compiler
CC = gcc
CXX = g++

# Compile options
CFLAGS = -Wall -O2
CXXFLAGS = -Wall -O2 -std=c++11

# Libraries
LIBS = `pkg-config --libs opencv`

# Library include path
INCLUDES = `pkg-config --cflags opencv`

# Source code file name
SRCS = $(wildcard *.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output executable file name
TARGET = $(SRCS:.cpp=)

default: $(TARGET)

# Pattern rules for executable files
$(TARGET): %: %.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS)

# Pattern rules for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Clean compiled files
clean:
	$(RM) $(TARGET) $(OBJS)

# Show the information of source files
show:
	@ls -l $(SRCS)
