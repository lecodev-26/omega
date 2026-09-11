#include "uart.h"
#include "exceptions.h"
#include "timer.h"
#include "gic.h"

void kmain(void) {
    uart_init();
    uart_puts("OMEGA boot minimal v5.1 (IRQ debug)\n");
    uart_puts("---\n");

    uart_puts("Inicializando excepciones...\n");
    exceptions_init();
    uart_puts("Vector table instalada.\n");

    uart_puts("Inicializando GIC...\n");
    gic_init();
    uart_puts("GIC inicializado.\n");

    uart_puts("Habilitando IRQ del timer (PPI ");
    uart_putdec32(TIMER_IRQ);
    uart_puts(")...\n");
    gic_enable_irq(TIMER_IRQ);
    uart_puts("IRQ habilitada.\n");

    uart_puts("Programando timer para 100 ms...\n");
    uint64_t freq = timer_frequency();
    uint64_t delta = freq / 10;   /* 100 ms */
    timer_schedule_irq(delta);
    timer_enable();
    uart_puts("Timer programado.\n");

    uart_puts("Habilitando IRQs en el CPU...\n");
    gic_enable_cpu_irqs();
    uart_puts("IRQs habilitadas.\n");
    uart_puts("---\n");
    uart_puts("Bucle de polling. Imprime estado cada ~10M nops.\n");

    /* Bucle infinito que imprime periódicamente el contador de IRQs */
    uint64_t last_count = 0;
    uint64_t iter = 0;
    for (;;) {
        __asm__ volatile("nop");

        /* Cada ~10M iteraciones, imprimir estado */
        iter++;
        if (iter % 10000000ULL == 0) {
            uart_puts("irq_count=");
            uart_putdec64(timer_irq_count());
            uart_puts(" irq_total=");
            uart_putdec64(exceptions_irq_count());
            uart_puts("\n");
        }

        /* Si llegamos a 5 ticks, detener y salir */
        if (timer_irq_count() >= 5 && last_count < 5) {
            last_count = timer_irq_count();
            uart_puts("*** Primeros 5 ticks alcanzados ***\n");
            break;
        }
    }

    /* Deshabilitar todo */
    timer_disable();
    gic_disable_cpu_irqs();

    uart_puts("---\n");
    uart_puts("Ticks recibidos: ");
    uart_putdec64(timer_irq_count());
    uart_puts("\n");
    uart_puts("IRQs totales: ");
    uart_putdec64(exceptions_irq_count());
    uart_puts("\n");
    uart_puts("=== fin ===\n");

    for (;;) {
        char c = uart_getc();
        if (c == 'q' || c == 'Q') {
            uart_puts("Saliendo...\n");
            break;
        }
    }

    for (;;) {
        __asm__ volatile("wfe");
    }
}
