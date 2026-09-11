# OMEGA — Kernel Makefile

CC      := clang
LD      := ld.lld
OBJCOPY := llvm-objcopy

TARGET  := aarch64-unknown-none
CFLAGS  := --target=$(TARGET) -std=c11 -Wall -Wextra -O2 -ffreestanding \
           -fno-stack-protector -mgeneral-regs-only -Iinclude
LDFLAGS := -T linker.ld

BUILD_DIR := build

ASM_SRCS := arch/arm64/boot.S \
            arch/arm64/exceptions_asm.S \
            arch/arm64/switch.S
C_SRCS   := kmain.c \
            arch/arm64/uart.c \
            arch/arm64/exceptions.c \
            arch/arm64/timer.c \
            arch/arm64/gic.c \
            arch/arm64/task.c \
            arch/arm64/ipc.c

ASM_OBJS := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SRCS))
C_OBJS   := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SRCS))
OBJS     := $(ASM_OBJS) $(C_OBJS)

BIN     := $(BUILD_DIR)/omega.elf
BIN_RAW := $(BUILD_DIR)/omega.bin

.PHONY: all clean inspect run

all: $(BIN) $(BIN_RAW)

$(BUILD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
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
