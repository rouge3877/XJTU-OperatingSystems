# Compiler and flags
CC = gcc
CFLAGS = -Wall -I. # Include the root directory for API.h
DEBUG_FLAGS = -ggdb -DDEBUG -Og # Debug-specific flags

# Root source files
SRC_ROOT = API.c
OBJS_ROOT = $(BUILD_DIR)/$(SRC_ROOT:.c=.o)

# Subdirectories
SUBDIRS = MemAllocate PageSwapping Pipe SoftInterrupt
BUILD_DIR = build

# Find all .c files in subdirectories
SUBDIR_SRCS = $(wildcard $(foreach dir, $(SUBDIRS), $(dir)/*.c))
SUBDIR_OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SUBDIR_SRCS))

# Generate executable names
EXECUTABLES = $(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%,$(SUBDIR_OBJS))

# Default target: build all executables
.PHONY: all clean debug list help

all: $(BUILD_DIR) $(EXECUTABLES)
	@echo "All executables built in '$(BUILD_DIR)' directory."

# Create build directory and subdirectories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(foreach dir, $(SUBDIRS), $(BUILD_DIR)/$(dir))

# Compile API.c
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile .c files in subdirectories
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create executables
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(OBJS_ROOT) | $(BUILD_DIR)
	@mkdir -p $(dir $@) # Ensure the directory exists
	$(CC) $(CFLAGS) $^ -o $@

# Debug mode: build all executables with debug flags
debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

page:
	@./$(BUILD_DIR)/PageSwapping/sim-page

pipe:
	@./$(BUILD_DIR)/Pipe/Pipe4IPC

ulkpipe:
	@./$(BUILD_DIR)/Pipe/ulk_Pipe4IPC

signal:
	@./$(BUILD_DIR)/SoftInterrupt/kill

malloc:
	@./$(BUILD_DIR)/MemAllocate/Memalloc

# Clean up all build files
clean:
	rm -rf $(BUILD_DIR)

# List all generated files
list:
	@echo "Generated executables:"
	@echo $(EXECUTABLES)

