#include "omega/uart.h"
#include "omega/exceptions.h"
#include "omega/timer.h"
#include "omega/gic.h"
#include "omega/task.h"
#include "omega/ipc.h"
#include "omega/cap.h"

/* Variables de conteo para verificar progreso */
static volatile uint64_t g_a_count = 0;
static volatile uint64_t g_b_count = 0;

static void task_a(void) {
    for (;;) {
        uart_puts("[A]");
        g_a_count++;
        /* Sin task_yield: el timer debe forzar el cambio */
        /* Un bucle pequeño para que la IRQ pueda dispararse */
        for (volatile int i = 0; i < 100000; i++) {}
    }
}

static void task_b(void) {
    for (;;) {
        uart_puts("[B]");
        g_b_count++;
        for (volatile int i = 0; i < 100000; i++) {}
    }
}

void kmain(void) {
    uart_init();
    uart_puts("OMEGA kernel v11 (preemption)\n");
    uart_puts("---\n");

    uart_puts("Inicializando excepciones...\n");
    exceptions_init();
    uart_puts("Vector table instalada.\n");

    uart_puts("Inicializando GIC...\n");
    gic_init();
    uart_puts("Habilitando IRQ del timer (PPI ");
    uart_putdec32(TIMER_IRQ);
    uart_puts(")...\n");
    gic_enable_irq(TIMER_IRQ);

    uart_puts("Programando timer para 100 ms...\n");
    uint64_t freq = timer_frequency();
    uint64_t delta = freq / 10;   /* 100 ms */
    timer_schedule_irq(delta);
    timer_enable();
    uart_puts("Timer programado.\n");

    uart_puts("Inicializando IPC...\n");
    ipc_init();

    uart_puts("Inicializando capabilities...\n");
    cap_init();

    uart_puts("Inicializando tareas...\n");
    task_init();

    task_create("A", task_a);
    task_create("B", task_b);

    uart_puts("Tareas creadas: A=0, B=1\n");

    uart_puts("Habilitando IRQs en el CPU...\n");
    gic_enable_cpu_irqs();
    uart_puts("IRQs habilitadas.\n");

    uart_puts("---\n");
    uart_puts("Iniciando scheduler (preemptivo)...\n");

    /* Arrancar la primera tarea. Nunca retorna. */
    task_yield();

    for (;;) {
        __asm__ volatile("wfe");
    }
}
