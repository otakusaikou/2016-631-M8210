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

# Output executable files
TARGET = rectDetect

# Output executable file names
default: $(TARGET)

# Rule for executable files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) $(LDFLAGS)

# Pattern rules for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Clean compiled files
clean:
	$(RM) $(TARGET) $(OBJS)

# Show the information of source files
show:
	@ls -l $(SRCS)
