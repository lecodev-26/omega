# OMEGA — Host prototype Makefile
#
# Build system para el prototipo host. Compilable en Termux con clang.

CC      ?= clang
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS :=

# === ARM64 / aarch64 ===
ARCH ?= native
ARM64_CFLAGS := -march=armv8-a -mtune=cortex-a53

SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

MAIN_SRC := $(SRC_DIR)/main.c
LIB_SRCS := $(filter-out $(MAIN_SRC),$(SRCS))
LIB_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_SRCS))

BIN := $(BUILD_DIR)/omega-host

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%,$(TEST_SRCS))

.PHONY: all test clean run arm64

all: $(BIN)

arm64: CFLAGS += $(ARM64_CFLAGS)
arm64: clean all
	@echo "=== ARM64 build ==="
	@file $(BIN)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%: $(TEST_DIR)/%.c $(LIB_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LIB_OBJS) -o $@ $(LDFLAGS)

test: $(TEST_BINS)
	@echo "=== Running tests ==="
	@for t in $(TEST_BINS); do \
		echo "--- $$t ---"; \
		$$t || exit 1; \
	done
	@echo "=== All tests passed ==="

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)
