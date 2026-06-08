# Makefile for lexer unit tests with Google Test
#
# Override GTEST_PREFIX, GTEST_INCLUDE_DIR, or GTEST_LIB_DIR when gtest is
# installed in a non-standard location.

CXX = g++
CXXFLAGS = -std=c++20 -Wall -g -fPIC
GTEST_PREFIX ?= $(shell if command -v brew >/dev/null 2>&1; then brew --prefix googletest 2>/dev/null; fi)
GTEST_INCLUDE_DIR ?= $(if $(GTEST_PREFIX),$(GTEST_PREFIX)/include,)
GTEST_LIB_DIR ?= $(if $(GTEST_PREFIX),$(GTEST_PREFIX)/lib,)
INCLUDES = -I../include -I../include/lexer $(if $(GTEST_INCLUDE_DIR),-I$(GTEST_INCLUDE_DIR),)
LDFLAGS = $(if $(GTEST_LIB_DIR),-L$(GTEST_LIB_DIR),)
GTEST_FLAGS = 

# Directories
SRC_DIR = ../src
LEXER_DIR = $(SRC_DIR)/lexer
TEST_DIR = .

# Source files from lexer
LEXER_SOURCES = $(LEXER_DIR)/dfa.cpp \
                $(LEXER_DIR)/state.cpp \
                $(LEXER_DIR)/state_list.cpp \
                $(LEXER_DIR)/trie_node.cpp

LEXER_OBJECTS = $(LEXER_SOURCES:.cpp=.o)

# Test file
TEST_SOURCES = $(TEST_DIR)/lexer_test.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

# Output
TEST_EXECUTABLE = lexer_test

# Google Test library
GTEST_LIB = -lgtest -lgtest_main -pthread

# Default target
all: $(TEST_EXECUTABLE)

# Build the test executable
$(TEST_EXECUTABLE): $(LEXER_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(GTEST_LIB)

# Build object files from lexer
$(LEXER_DIR)/%.o: $(LEXER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build test object file
$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GTEST_FLAGS) -c $< -o $@

# Run tests
run: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

# Clean up
clean:
	rm -f $(LEXER_OBJECTS) $(TEST_OBJECTS) $(TEST_EXECUTABLE)
	rm -f ../src/lexer/*.d
	rm -f *.o

.PHONY: all run clean
