#include "uart.h"
#include "exceptions.h"
#include "timer.h"

void kmain(void) {
    uart_init();
    uart_puts("OMEGA boot minimal v4 (timer)\n");
    uart_puts("---\n");

    uart_puts("Inicializando excepciones...\n");
    exceptions_init();
    uart_puts("Vector table instalada.\n");
    uart_puts("---\n");

    uart_puts("Timer:\n");
    uint64_t freq = timer_frequency();
    uart_puts("  frecuencia = ");
    uart_putdec64(freq);
    uart_puts(" Hz\n");

    uart_puts("  midiendo 100 ms...\n");
    uint64_t t0 = timer_read_counter();
    timer_delay_us(100000);
    uint64_t t1 = timer_read_counter();

    uint64_t delta_ticks = t1 - t0;
    uint64_t delta_us = (delta_ticks * 1000000ULL) / freq;

    uart_puts("  t0 = ");
    uart_putdec64(t0);
    uart_puts(" ticks\n");

    uart_puts("  t1 = ");
    uart_putdec64(t1);
    uart_puts(" ticks\n");

    uart_puts("  delta = ");
    uart_putdec64(delta_ticks);
    uart_puts(" ticks = ");
    uart_putdec64(delta_us);
    uart_puts(" us\n");

    uart_puts("---\n");
    uart_puts("Todo OK. Envia 'q' para salir.\n");
    uart_puts("=== fin ===\n");

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

    for (;;) {
        __asm__ volatile("wfe");
    }
}
