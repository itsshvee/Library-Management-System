CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude -O2

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
TEST_DIR = tests

SOURCES = $(SRC_DIR)/Book.cpp \
          $(SRC_DIR)/Transaction.cpp \
          $(SRC_DIR)/FileManager.cpp \
          $(SRC_DIR)/Library.cpp

MAIN_SRC = $(SRC_DIR)/main.cpp
TEST_SRC = $(TEST_DIR)/test_runner.cpp

OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
MAIN_OBJ = $(OBJ_DIR)/main.o
TEST_OBJ = $(OBJ_DIR)/test_runner.o

TARGET = LibraryManagementSystem
TEST_TARGET = test_runner

.PHONY: all clean test run

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/test_runner.o: $(TEST_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)