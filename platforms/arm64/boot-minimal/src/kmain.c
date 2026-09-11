/*
 * OMEGA — Boot minimal
 * kmain.c
 *
 * Punto de entrada en C.
 * Este NO es el kernel de OMEGA. Es un binario mínimo que arranca
 * en bare-metal y escribe por UART.
 */

#include "uart.h"

void kmain(void) {
    uart_init();
    uart_puts("OMEGA boot minimal\n");
    uart_puts("Hello from aarch64 bare-metal\n");
    uart_puts("---\n");
    uart_puts("Esto NO es el kernel de OMEGA.\n");
    uart_puts("Es un binario minimo de prueba.\n");
    uart_puts("=== fin ===\n");

    /* Loop infinito para que QEMU no salga */
    while (1) {
        __asm__ volatile("wfe");
    }
}
