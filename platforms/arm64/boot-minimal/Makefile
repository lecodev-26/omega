# OMEGA — Boot minimal Makefile

CC      := clang
LD      := ld.lld
OBJCOPY := llvm-objcopy

TARGET  := aarch64-unknown-none
# -mgeneral-regs-only: prohibido usar FP/SIMD (no estan habilitados por defecto)
CFLAGS  := --target=$(TARGET) -std=c11 -Wall -Wextra -O2 -ffreestanding \
           -fno-stack-protector -mgeneral-regs-only -Iinclude
LDFLAGS := -T linker.ld

SRC_DIR   := src
BUILD_DIR := build

ASM_SRCS := boot.S exceptions_asm.S
C_SRCS   := kmain.c uart.c exceptions.c

ASM_OBJS := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SRCS))
C_OBJS   := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SRCS))
OBJS     := $(ASM_OBJS) $(C_OBJS)

BIN     := $(BUILD_DIR)/omega-boot.elf
BIN_RAW := $(BUILD_DIR)/omega-boot.bin

.PHONY: all clean inspect run

all: $(BIN) $(BIN_RAW)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
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
