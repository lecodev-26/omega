/*
 * OMEGA — Boot minimal
 * uart.h — Driver UART PL011 (QEMU virt)
 */

#ifndef OMEGA_BOOT_UART_H
#define OMEGA_BOOT_UART_H

#include <stdint.h>

#define UART0_BASE  0x09000000

#define UART_DR     (UART0_BASE + 0x00)  /* Data Register */
#define UART_FR     (UART0_BASE + 0x18)  /* Flag Register */

/* Flags del registro FR */
#define UART_FR_TXFF (1 << 5)  /* TX FIFO full */
#define UART_FR_RXFE (1 << 4)  /* RX FIFO empty */

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);

/*
 * Lee un carácter del UART (blocking).
 * No usa interrupciones; hace polling hasta que llega un byte.
 */
char uart_getc(void);

#endif /* OMEGA_BOOT_UART_H */
