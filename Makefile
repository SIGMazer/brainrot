# Compiler and tool configurations
CC := gcc
BISON := bison
FLEX := flex
PYTHON := python3

# Compiler and linker flags
CFLAGS := -Wall -Wextra -O2
LDFLAGS := -lfl -lm

# Source files and directories
SRC_DIR := lib
SRCS := $(SRC_DIR)/hm.c $(SRC_DIR)/mem.c ast.c
GENERATED_SRCS := lang.tab.c lex.yy.c
ALL_SRCS := $(SRCS) $(GENERATED_SRCS)

# Output files
TARGET := brainrot
BISON_OUTPUT := lang.tab.c
FLEX_OUTPUT := lex.yy.c

# Default target
.PHONY: all
all: $(TARGET)

# Main executable build
$(TARGET): $(ALL_SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Generate parser files using Bison
$(BISON_OUTPUT): lang.y
	$(BISON) -d -Wcounterexamples $< -o $@

# Generate lexer files using Flex
$(FLEX_OUTPUT): lang.l
	$(FLEX) $<

# Run tests
.PHONY: test
test:
	$(PYTHON) -m pytest -v

# Clean build artifacts
.PHONY: clean
clean:
	rm -f $(TARGET) $(GENERATED_SRCS) lang.tab.h
	rm -f *.o

# Check dependencies
.PHONY: check-deps
check-deps:
	@command -v $(CC) >/dev/null 2>&1 || { echo "Error: gcc not found"; exit 1; }
	@command -v $(BISON) >/dev/null 2>&1 || { echo "Error: bison not found"; exit 1; }
	@command -v $(FLEX) >/dev/null 2>&1 || { echo "Error: flex not found"; exit 1; }
	@command -v $(PYTHON) >/dev/null 2>&1 || { echo "Error: python3 not found"; exit 1; }
	@$(PYTHON) -c "import pytest" >/dev/null 2>&1 || { echo "Error: pytest not found. Install with: pip install pytest"; exit 1; }

# Development helper to rebuild everything from scratch
.PHONY: rebuild
rebuild: clean all

# Format source files (requires clang-format)
.PHONY: format
format:
	@command -v clang-format >/dev/null 2>&1 || { echo "Error: clang-format not found"; exit 1; }
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i

# Show help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all        : Build the main executable (default target)"
	@echo "  test       : Run the test suite"
	@echo "  clean      : Remove all generated files"
	@echo "  check-deps : Verify all required dependencies are installed"
	@echo "  rebuild    : Clean and rebuild the project"
	@echo "  format     : Format source files using clang-format"
	@echo "  help       : Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  CC        = $(CC)"
	@echo "  CFLAGS    = $(CFLAGS)"
	@echo "  LDFLAGS   = $(LDFLAGS)"
