#include "omega/uart.h"
#include "omega/exceptions.h"
#include "omega/timer.h"
#include "omega/gic.h"
#include "omega/task.h"

static void task_a(void) {
    for (;;) {
        uart_puts("[A]");
        for (volatile int i = 0; i < 1000000; i++) {}
        task_yield();
    }
}

static void task_b(void) {
    for (;;) {
        uart_puts("[B]");
        for (volatile int i = 0; i < 1000000; i++) {}
        task_yield();
    }
}

void kmain(void) {
    uart_init();
    uart_puts("OMEGA kernel v6 (tasks)\n");
    uart_puts("---\n");

    uart_puts("Inicializando excepciones...\n");
    exceptions_init();
    uart_puts("Vector table instalada.\n");

    uart_puts("Inicializando tareas...\n");
    task_init();

    int ta = task_create("A", task_a);
    int tb = task_create("B", task_b);

    uart_puts("Tareas creadas: ");
    uart_putdec32(task_count());
    uart_puts(" (A=");
    uart_putdec32(ta);
    uart_puts(", B=");
    uart_putdec32(tb);
    uart_puts(")\n");

    uart_puts("---\n");
    uart_puts("Iniciando scheduler...\n");

    /* Arrancar la primera tarea */
    task_yield();

    /* No debería llegar aquí */
    uart_puts("ERROR: el scheduler retornó\n");
    for (;;) {
        __asm__ volatile("wfe");
    }
}
