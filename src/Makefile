# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O3

# Tools
BISON = bison
FLEX = flex

# File names
BISON_FILE = parser.y
FLEX_FILE = lexer.l
MAIN_FILE = main.cpp

# Generated files
BISON_CPP = parser.tab.cpp
BISON_HEADER = parser.tab.hpp
FLEX_CPP = lexer.yy.cpp

SRC_FILES = instructions.cpp

OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Output executable
EXEC = compiler

# Rules
all: $(EXEC)

$(EXEC): $(BISON_CPP) $(FLEX_CPP) $(OBJ_FILES) $(MAIN_FILE)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BISON_CPP) $(BISON_HEADER): $(BISON_FILE)
	$(BISON) -d -o $(BISON_CPP) $(BISON_FILE)

$(FLEX_CPP): $(FLEX_FILE) $(BISON_HEADER)
	$(FLEX) -o $(FLEX_CPP) $(FLEX_FILE)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BISON_CPP) $(BISON_HEADER) $(FLEX_CPP) $(OBJ_FILES) $(EXEC)

.PHONY: all clean