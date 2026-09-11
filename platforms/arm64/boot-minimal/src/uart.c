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
