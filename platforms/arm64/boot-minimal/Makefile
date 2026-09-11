# OMEGA — Boot minimal Makefile

CC      := clang
LD      := ld.lld
OBJCOPY := llvm-objcopy

# Target bare-metal aarch64
TARGET  := aarch64-unknown-none
CFLAGS  := --target=$(TARGET) -std=c11 -Wall -Wextra -O2 -ffreestanding -fno-stack-protector -Iinclude
LDFLAGS := -T linker.ld

SRC_DIR   := src
BUILD_DIR := build

OBJS := $(BUILD_DIR)/boot.o $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

BIN     := $(BUILD_DIR)/omega-boot.elf
BIN_RAW := $(BUILD_DIR)/omega-boot.bin

.PHONY: all clean inspect run

all: $(BIN) $(BIN_RAW)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

$(BIN_RAW): $(BIN)
	$(OBJCOPY) -O binary $< $@

inspect: $(BIN)
	@echo "=== file ==="
	@file $(BIN)
	@echo "=== readelf header ==="
	@llvm-readelf -h $(BIN) | head -20

run: $(BIN)
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a53 \
		-nographic \
		-kernel $(BIN)

clean:
	rm -rf $(BUILD_DIR)
