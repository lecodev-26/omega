#include "omega/uart.h"
#include "omega/exceptions.h"
#include "omega/timer.h"
#include "omega/gic.h"
#include "omega/task.h"
#include "omega/ipc.h"
#include "omega/cap.h"

/*
 * Concede una capability a la tarea actual que designa un endpoint.
 */
static void grant_endpoint_cap(uint64_t endpoint_id, uint64_t rights) {
    capability_t cap;
    cap.object_id  = endpoint_id;
    cap.rights     = rights;
    cap.generation = 0;
    cap.valid      = 1;
    int idx = cap_add(&cap);
    if (idx < 0) {
        uart_puts("ERROR: no se pudo conceder cap\n");
    }
}

static void task_a(void) {
    /* Tarea A: conceder cap al endpoint 1 (B) */
    grant_endpoint_cap(1, CAP_RIGHT_WRITE | CAP_RIGHT_READ);
    uart_puts("[A: cap a endpoint 1 concedida]");

    /* Probar que A NO tiene cap al endpoint 99 (inexistente) */
    if (cap_lookup(99, CAP_RIGHT_WRITE) < 0) {
        uart_puts("[A: cap a 99 NO existe (correcto)]");
    }

    for (;;) {
        ipc_message_t msg;
        msg.sender = 0;
        msg.type = IPC_MSG_PING;
        msg.length = 0;

        if (ipc_send(1, &msg) == 0) {
            uart_puts("[A->B ping]");
        } else {
            uart_puts("[A: send denegado]");
        }

        ipc_message_t in;
        if (ipc_recv(&in) == 0) {
            uart_puts("[A<-B pong]");
        }

        for (volatile int i = 0; i < 500000; i++) {}
        task_yield();
    }
}

static void task_b(void) {
    /* Tarea B: conceder cap al endpoint 0 (A) */
    grant_endpoint_cap(0, CAP_RIGHT_WRITE | CAP_RIGHT_READ);
    uart_puts("[B: cap a endpoint 0 concedida]");

    for (;;) {
        ipc_message_t in;
        if (ipc_recv(&in) == 0) {
            if (in.type == IPC_MSG_PING) {
                ipc_message_t pong;
                pong.sender = 1;
                pong.type = IPC_MSG_PONG;
                pong.length = 0;
                if (ipc_send(0, &pong) == 0) {
                    uart_puts("[B: pong enviado]");
                }
            }
        }

        for (volatile int i = 0; i < 500000; i++) {}
        task_yield();
    }
}

void kmain(void) {
    uart_init();
    uart_puts("OMEGA kernel v8 (capabilities)\n");
    uart_puts("---\n");

    uart_puts("Inicializando excepciones...\n");
    exceptions_init();
    uart_puts("Vector table instalada.\n");

    uart_puts("Inicializando IPC...\n");
    ipc_init();

    uart_puts("Inicializando capabilities...\n");
    cap_init();

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
