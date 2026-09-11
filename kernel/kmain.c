#include "omega/uart.h"
#include "omega/exceptions.h"
#include "omega/timer.h"
#include "omega/gic.h"
#include "omega/task.h"
#include "omega/ipc.h"

static int g_ping_pong_count = 0;

static void task_a(void) {
    ipc_message_t msg;
    msg.sender = 0;
    msg.type = IPC_MSG_PING;
    msg.length = 0;

    for (;;) {
        /* Enviar PING a tarea B (endpoint 1) */
        if (ipc_send(1, &msg) == 0) {
            uart_puts("[A->B ping]");
        }

        /* Recibir respuesta de B */
        ipc_message_t in;
        if (ipc_recv(&in) == 0) {
            uart_puts("[A<-B pong]");
        }

        /* Ceder el control */
        for (volatile int i = 0; i < 500000; i++) {}
        task_yield();
    }
}

static void task_b(void) {
    for (;;) {
        /* Recibir mensaje de A */
        ipc_message_t in;
        if (ipc_recv(&in) == 0) {
            if (in.type == IPC_MSG_PING) {
                /* Responder con PONG a tarea A (endpoint 0) */
                ipc_message_t pong;
                pong.sender = 1;
                pong.type = IPC_MSG_PONG;
                pong.length = 0;
                if (ipc_send(0, &pong) == 0) {
                    uart_puts("[B: pong enviado]");
                    g_ping_pong_count++;
                }
            }
        }

        for (volatile int i = 0; i < 500000; i++) {}
        task_yield();
    }
}

void kmain(void) {
    uart_init();
    uart_puts("OMEGA kernel v7 (IPC)\n");
    uart_puts("---\n");

    uart_puts("Inicializando excepciones...\n");
    exceptions_init();
    uart_puts("Vector table instalada.\n");

    uart_puts("Inicializando IPC...\n");
    ipc_init();

    uart_puts("Inicializando tareas...\n");
    task_init();

    int ta = task_create("A", task_a);
    int tb = task_create("B", task_b);

    uart_puts("Tareas creadas: A=");
    uart_putdec32(ta);
    uart_puts(", B=");
    uart_putdec32(tb);
    uart_puts("\n");

    uart_puts("---\n");
    uart_puts("Iniciando scheduler...\n");

    task_yield();

    for (;;) {
        __asm__ volatile("wfe");
    }
}
