/*
 * OMEGA — Kernel
 * kmain.c — Punto de entrada
 *
 * Test actual: IPC blocking con preemption.
 * Tarea A (pinger) y B (ponger) se mandan mensajes sin ceder
 * explícitamente el control. La preemption y el blocking hacen
 * el trabajo.
 */

#include "omega/uart.h"
#include "omega/exceptions.h"
#include "omega/timer.h"
#include "omega/gic.h"
#include "omega/task.h"
#include "omega/ipc.h"
#include "omega/cap.h"

static volatile uint64_t g_a_count = 0;
static volatile uint64_t g_b_count = 0;

static void task_a(void) {
    ipc_message_t msg;
    int count = 0;

    for (;;) {
        /* Enviar PING a B (endpoint 1) */
        msg.sender = 0;
        msg.type = IPC_MSG_PING;
        msg.length = 0;
        msg.has_cap = 0;

        ipc_send(1, &msg);

        /* Recibir PONG de B */
        ipc_message_t in;
        if (ipc_recv(&in) == 0 && in.type == IPC_MSG_PONG) {
            count++;
            g_a_count++;
            if (count <= 5) {
                uart_puts("[A:PONG");
                uart_putdec32((uint32_t)count);
                uart_puts("]");
            }
        }
    }
}

static void task_b(void) {
    for (;;) {
        /* Recibir PING de A */
        ipc_message_t in;
        if (ipc_recv(&in) == 0 && in.type == IPC_MSG_PING) {
            g_b_count++;

            /* Enviar PONG a A (endpoint 0) */
            ipc_message_t msg;
            msg.sender = 1;
            msg.type = IPC_MSG_PONG;
            msg.length = 0;
            msg.has_cap = 0;

            ipc_send(0, &msg);

            if (g_b_count <= 5) {
                uart_puts("[B:PING]");
            }
        }
    }
}

void kmain(void) {
    uart_init();
    uart_puts("OMEGA kernel v12 (IPC blocking test)\n");
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
    uint64_t delta = freq / 10;
    timer_schedule_irq(delta);
    timer_enable();
    uart_puts("Timer programado.\n");

    uart_puts("Inicializando IPC...\n");
    ipc_init();

    uart_puts("Inicializando capabilities...\n");
    cap_init();

    uart_puts("Inicializando tareas...\n");
    task_init();

    int a = task_create("A", task_a);
    int b = task_create("B", task_b);
    uart_puts("Tareas creadas: A=");
    uart_putdec32((uint32_t)a);
    uart_puts(", B=");
    uart_putdec32((uint32_t)b);
    uart_puts("\n");

    /*
     * Conceder capabilities de IPC:
     *   A (endpoint 0) necesita WRITE sobre endpoint 1.
     *   B (endpoint 1) necesita WRITE sobre endpoint 0.
     */
    uart_puts("Concediendo capabilities de IPC...\n");

    capability_t cap_a;
    cap_a.object_id = 1;                  /* endpoint B */
    cap_a.rights    = CAP_RIGHT_WRITE;
    cap_a.generation = 0;
    cap_a.valid     = 1;
    cap_add_to(0, &cap_a);

    capability_t cap_b;
    cap_b.object_id = 0;                  /* endpoint A */
    cap_b.rights    = CAP_RIGHT_WRITE;
    cap_b.generation = 0;
    cap_b.valid     = 1;
    cap_add_to(1, &cap_b);

    uart_puts("Capabilities concedidas.\n");

    uart_puts("Habilitando IRQs en el CPU...\n");
    gic_enable_cpu_irqs();
    uart_puts("IRQs habilitadas.\n");

    uart_puts("---\n");
    uart_puts("Iniciando scheduler...\n");

    task_yield();

    for (;;) {
        __asm__ volatile("wfe");
    }
}
