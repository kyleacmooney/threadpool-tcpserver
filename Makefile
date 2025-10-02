# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS = -lpthread

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/server

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Rebuild
rebuild: clean all

# Run server
run: all
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: rebuild

# Release build
release: CXXFLAGS += -O3 -DNDEBUG
release: rebuild

.PHONY: all clean rebuild run debug release directories
