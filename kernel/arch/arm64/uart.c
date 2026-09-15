/*
 * OMEGA — Kernel
 * uart.c — UART PL011 (QEMU virt)
 *
 * La UART está protegida por un spinlock que deshabilita IRQs.
 * Esto evita que el handler de IRQ del timer interrumpa mientras
 * se está imprimiendo y cause basura o deadlock.
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

/* Estado del lock */
static volatile int g_uart_lock = 0;

void uart_lock(void) {
    /* Deshabilitar IRQs (DAIF.I = 1) */
    __asm__ volatile("msr daifset, #2" ::: "memory");

    int tmp;
    do {
        __asm__ volatile(
            "ldaxr w1, [%1]\n"
            "cbnz w1, 1f\n"
            "mov w1, #1\n"
            "stlxr w2, w1, [%1]\n"
            "cbnz w2, 1f\n"
            "b 2f\n"
            "1:\n"
            "wfe\n"
            "2:\n"
            "mov %w0, w2\n"
            : "=r"(tmp)
            : "r"(&g_uart_lock)
            : "w1", "w2", "memory");
    } while (tmp != 0);
}

void uart_unlock(void) {
    __asm__ volatile(
        "stlr wzr, [%0]\n"
        :
        : "r"(&g_uart_lock)
        : "memory");

    /* Rehabilitar IRQs (DAIF.I = 0) */
    __asm__ volatile("msr daifclr, #2" ::: "memory");
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
    uart_lock();
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
    uart_unlock();
}

void uart_puthex64(uint64_t v) {
    uart_lock();
    uart_putc('0');
    uart_putc('x');
    for (int i = 60; i >= 0; i -= 4) {
        uint32_t nibble = (v >> i) & 0xF;
        char c = (nibble < 10) ? ('0' + nibble) : ('a' + nibble - 10);
        uart_putc(c);
    }
    uart_unlock();
}

void uart_putdec32(uint32_t v) {
    uart_lock();
    if (v == 0) {
        uart_putc('0');
        uart_unlock();
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
    uart_unlock();
}
