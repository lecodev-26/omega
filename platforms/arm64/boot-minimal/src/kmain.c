/*
 * OMEGA — Boot minimal
 * kmain.c (v2)
 *
 * Punto de entrada en C.
 * Este NO es el kernel de OMEGA. Es un binario mínimo que arranca
 * en bare-metal y escribe por UART.
 *
 * Añade: bucle de recepción UART para poder salir limpiamente.
 * Envía 'q' para terminar.
 */

#include "uart.h"

void kmain(void) {
    uart_init();
    uart_puts("OMEGA boot minimal\n");
    uart_puts("Hello from aarch64 bare-metal\n");
    uart_puts("---\n");
    uart_puts("Esto NO es el kernel de OMEGA.\n");
    uart_puts("Es un binario minimo de prueba.\n");
    uart_puts("Envia 'q' para salir.\n");
    uart_puts("=== fin ===\n");

    /* Bucle de espera activa por UART */
    for (;;) {
        char c = uart_getc();
        if (c == 'q' || c == 'Q') {
            uart_puts("Saliendo...\n");
            break;
        }
        if (c == '\r' || c == '\n') {
            uart_puts("\n");
        } else if (c >= 32 && c < 127) {
            uart_putc(c);
        }
    }

    /* Detener la CPU */
    for (;;) {
        __asm__ volatile("wfe");
    }
}
