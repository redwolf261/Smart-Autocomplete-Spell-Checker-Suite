# Makefile for Smart Autocomplete & Spell-Checker Suite

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TEST_DIR = tests
DATA_DIR = data

# Source files
SOURCES = $(SRC_DIR)/trie.cpp \
          $(SRC_DIR)/bloomfilter.cpp \
          $(SRC_DIR)/hashtable.cpp \
          $(SRC_DIR)/lrucache.cpp \
          $(SRC_DIR)/priorityqueue.cpp \
          $(SRC_DIR)/bktree.cpp \
          $(SRC_DIR)/editdistance.cpp \
          $(SRC_DIR)/spellchecker.cpp \
          $(SRC_DIR)/filehandler.cpp \
          $(SRC_DIR)/utils.cpp \
          $(SRC_DIR)/main.cpp

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Test files
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/test_%.o,$(TEST_SOURCES))
TEST_BINS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%,$(TEST_SOURCES))

# Target executable
TARGET = autocomplete

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build successful! Run with: ./$(TARGET)"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling test $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build individual test executables
$(BUILD_DIR)/test_%: $(BUILD_DIR)/test_%.o $(filter-out $(BUILD_DIR)/main.o,$(OBJECTS))
	@echo "Linking test $@..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Run all tests
test: $(TEST_BINS)
	@echo "Running all tests..."
	@for test in $(TEST_BINS); do \
		echo ""; \
		echo "=== Running $$test ==="; \
		./$$test || exit 1; \
	done
	@echo ""
	@echo "All tests passed!"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/test_* $(TARGET)
	@echo "Clean complete!"

# Clean everything including data files
cleanall: clean
	rm -f $(DATA_DIR)/query.txt $(DATA_DIR)/output.json

# Create necessary directories
setup:
	@mkdir -p $(BUILD_DIR) $(DATA_DIR)

# Run the application
run: $(TARGET)
	./$(TARGET)

# Help target
help:
	@echo "Smart Autocomplete & Spell-Checker Suite - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make all      - Build the main application"
	@echo "  make test     - Build and run all unit tests"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make cleanall - Remove build artifacts and data files"
	@echo "  make run      - Build and run the application"
	@echo "  make setup    - Create necessary directories"
	@echo "  make help     - Show this help message"

.PHONY: all clean cleanall test run setup help
