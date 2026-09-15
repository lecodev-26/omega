/*
 * OMEGA — Kernel
 * uart.h — UART PL011 (QEMU virt)
 */

#ifndef OMEGA_UART_H
#define OMEGA_UART_H

#include <stdint.h>

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_puthex64(uint64_t v);
void uart_putdec32(uint32_t v);

/*
 * Spinlock de la UART.
 *
 * Protege la salida cuando varias tareas (o el handler de IRQ)
 * escriben a la UART a la vez. En un solo CPU, un simple
 * flag con instrucciones atómicas basta.
 */
void uart_lock(void);
void uart_unlock(void);

#endif /* OMEGA_UART_H */
