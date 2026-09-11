/*
 * OMEGA — Boot minimal
 * uart.h — Driver UART PL011 (QEMU virt)
 */

#ifndef OMEGA_BOOT_UART_H
#define OMEGA_BOOT_UART_H

#include <stdint.h>

/*
 * Dirección base del UART PL011 en QEMU virt.
 * Es un estándar para aarch64 virt machine.
 */
#define UART0_BASE  0x09000000

/* Registros del PL011 */
#define UART_DR     (UART0_BASE + 0x00)  /* Data Register */
#define UART_FR     (UART0_BASE + 0x18)  /* Flag Register */

/* Flag: transmit FIFO full */
#define UART_FR_TXFF (1 << 5)

/*
 * Inicializa el UART.
 * En QEMU virt ya está inicializado por el firmware, así que es no-op.
 */
void uart_init(void);

/*
 * Envía un carácter por el UART (blocking).
 */
void uart_putc(char c);

/*
 * Envía una cadena por el UART.
 */
void uart_puts(const char *s);

#endif /* OMEGA_BOOT_UART_H */
