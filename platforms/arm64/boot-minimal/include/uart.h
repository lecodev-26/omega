/*
 * OMEGA — Boot minimal
 * uart.h — Driver UART PL011 (QEMU virt)
 */

#ifndef OMEGA_BOOT_UART_H
#define OMEGA_BOOT_UART_H

#include <stdint.h>

#define UART0_BASE  0x09000000

#define UART_DR     (UART0_BASE + 0x00)
#define UART_FR     (UART0_BASE + 0x18)

#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
char uart_getc(void);

/*
 * Imprime un número en decimal.
 */
void uart_putdec64(uint64_t v);
void uart_putdec32(uint32_t v);

/*
 * Imprime un número en hexadecimal (64 bits).
 */
void uart_puthex64(uint64_t v);

#endif /* OMEGA_BOOT_UART_H */
