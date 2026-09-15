/*
 * OMEGA — Kernel
 * uart.c — UART PL011 (QEMU virt)
 *
 * Sincronización: en un sistema uniprocesador, basta con
 * deshabilitar IRQs mientras se imprime. No hay spinlock.
 */

#include "omega/uart.h"

#define UART_BASE  0x09000000
#define UART_DR    (UART_BASE + 0x00)
#define UART_FR    (UART_BASE + 0x18)

#define UART_FR_TXFF  (1u << 5)

static inline void mmio_write32(uintptr_t addr, uint32_t value) {
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t mmio_read32(uintptr_t addr) {
    return *(volatile uint32_t *)addr;
}

uint64_t uart_lock(void) {
    uint64_t daif;
    __asm__ volatile("mrs %0, daif" : "=r"(daif));
    __asm__ volatile("msr daifset, #2" ::: "memory");
    return daif;
}

void uart_unlock(uint64_t saved_daif) {
    __asm__ volatile("msr daif, %0" :: "r"(saved_daif) : "memory");
}

void uart_init(void) {
    mmio_write32(UART_BASE + 0x38, 0);
    mmio_write32(UART_BASE + 0x44, 0x7FF);
}

void uart_putc(char c) {
    while (mmio_read32(UART_FR) & UART_FR_TXFF) {
        /* spin */
    }
    mmio_write32(UART_DR, (uint32_t)(unsigned char)c);
}

void uart_puts(const char *s) {
    uint64_t daif = uart_lock();
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
    uart_unlock(daif);
}

void uart_puthex64(uint64_t v) {
    uint64_t daif = uart_lock();
    uart_putc('0');
    uart_putc('x');
    for (int i = 60; i >= 0; i -= 4) {
        uint32_t nibble = (v >> i) & 0xF;
        char c = (nibble < 10) ? ('0' + nibble) : ('a' + nibble - 10);
        uart_putc(c);
    }
    uart_unlock(daif);
}

void uart_putdec32(uint32_t v) {
    uint64_t daif = uart_lock();
    if (v == 0) {
        uart_putc('0');
        uart_unlock(daif);
        return;
    }
    char buf[10];
    int i = 0;
    while (v > 0 && i < 10) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }
    while (i > 0) {
        uart_putc(buf[--i]);
    }
    uart_unlock(daif);
}
