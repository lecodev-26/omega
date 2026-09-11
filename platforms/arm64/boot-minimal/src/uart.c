/*
 * OMEGA — Boot minimal
 * uart.c
 */

#include "uart.h"

static inline void mmio_write(uintptr_t addr, uint32_t value) {
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t mmio_read(uintptr_t addr) {
    return *(volatile uint32_t *)addr;
}

void uart_init(void) {
    /* No-op: QEMU virt ya inicializa el UART */
}

void uart_putc(char c) {
    while (mmio_read(UART_FR) & UART_FR_TXFF) {
        /* spin */
    }
    mmio_write(UART_DR, (uint32_t)c);
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

char uart_getc(void) {
    while (mmio_read(UART_FR) & UART_FR_RXFE) {
        /* spin */
    }
    return (char)(mmio_read(UART_DR) & 0xFF);
}

void uart_putdec64(uint64_t v) {
    char buf[21];  /* 20 dígitos max para 2^64 */
    int i = 0;

    if (v == 0) {
        uart_putc('0');
        return;
    }

    while (v > 0 && i < (int)sizeof(buf)) {
        buf[i++] = (char)('0' + (v % 10));
        v /= 10;
    }

    while (i > 0) {
        uart_putc(buf[--i]);
    }
}

void uart_putdec32(uint32_t v) {
    uart_putdec64((uint64_t)v);
}

void uart_puthex64(uint64_t v) {
    uart_puts("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uint32_t nib = (uint32_t)((v >> i) & 0xF);
        char c = (nib < 10) ? (char)('0' + nib) : (char)('a' + nib - 10);
        uart_putc(c);
    }
}
