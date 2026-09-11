# OMEGA — Pilot 1 IPC — Host simulation Makefile

CC      ?= clang
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2
CFLAGS  += -Iinclude -I../../../platforms/host/include
LDFLAGS :=

# Ruta al host prototype (relativa)
HOST_DIR := ../../../platforms/host

SRC_DIR   := src
BUILD_DIR := build

# Fuentes propias
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Fuentes del host prototype (excluyendo main.c)
HOST_LIB_SRCS := $(wildcard $(HOST_DIR)/src/*.c)
HOST_LIB_SRCS := $(filter-out $(HOST_DIR)/src/main.c,$(HOST_LIB_SRCS))
HOST_LIB_OBJS := $(patsubst $(HOST_DIR)/src/%.c,$(BUILD_DIR)/host_%.o,$(HOST_LIB_SRCS))

BIN := $(BUILD_DIR)/pilot1-host-sim

.PHONY: all clean run

all: $(BIN)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/host_%.o: $(HOST_DIR)/src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS) $(HOST_LIB_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(BIN)
	@mkdir -p results
	./$(BIN) results/pilot1_host.csv

clean:
	rm -rf $(BUILD_DIR) results
