#include "uart.h"
#include "exceptions.h"

void kmain(void) {
    uart_init();
    uart_puts("OMEGA boot minimal v3\n");
    uart_puts("---\n");

    uart_puts("Inicializando excepciones...\n");
    exceptions_init();
    uart_puts("Vector table instalada.\n");
    uart_puts("---\n");

    uart_puts("Prueba: BRK #0 (breakpoint instruction)\n");
    uart_puts("Antes del BRK...\n");

    /* Esto lanza una excepcion sincrona de tipo BRK.
     * NOTA: el compilador no puede optimizarlo porque es asm volatile. */
    __asm__ volatile("brk #0");

    /* No llegamos aqui si la excepcion funciona */
    uart_puts("ERROR: no se lanzo excepcion\n");

    for (;;) {
        char c = uart_getc();
        if (c == 'q' || c == 'Q') break;
    }

    for (;;) {
        __asm__ volatile("wfe");
    }
}
