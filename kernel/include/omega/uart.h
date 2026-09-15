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
 * uart_lock/uart_unlock:
 *
 * En un sistema uniprocesador, basta con deshabilitar IRQs
 * mientras se imprime. Guardamos el estado de DAIF para restaurarlo
 * después. No hay spinlock: no hay otro CPU que compita.
 */
uint64_t uart_lock(void);
void uart_unlock(uint64_t saved_daif);

#endif /* OMEGA_UART_H */
