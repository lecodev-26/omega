#include "omega/uart.h"
#include "omega/exceptions.h"
#include "omega/timer.h"
#include "omega/gic.h"
#include "omega/task.h"
#include "omega/ipc.h"
#include "omega/cap.h"

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
    /* Tarea A: cap al endpoint B (1) con WRITE|READ */
    grant_endpoint_cap(1, CAP_RIGHT_WRITE | CAP_RIGHT_READ);

    /* Cap "objeto 5" con READ|GRANT (esta se va a delegar) */
    grant_endpoint_cap(5, CAP_RIGHT_READ | CAP_RIGHT_GRANT);

    uart_puts("[A: caps concedidas]");

    /* Buscar el índice de la cap al objeto 5 para poder adjuntarla */
    int cap5_idx = cap_lookup(5, CAP_RIGHT_READ | CAP_RIGHT_GRANT);
    if (cap5_idx < 0) {
        uart_puts("[A: ERROR cap5 no encontrada]");
        for (;;) task_yield();
    }

    /* Enviar a B un mensaje adjuntando la cap al objeto 5 */
    ipc_message_t msg;
    msg.sender = 0;
    msg.type = IPC_MSG_DATA;
    msg.length = 3;
    msg.has_cap = 0;
    msg.payload[0] = 'H';
    msg.payload[1] = 'i';
    msg.payload[2] = '!';

    if (ipc_send_with_cap(1, &msg, cap5_idx) == 0) {
        uart_puts("[A: msg+cap enviado a B]");
    } else {
        uart_puts("[A: ERROR enviando msg+cap]");
    }

    for (;;) {
        task_yield();
    }
}

static void task_b(void) {
    /* Tarea B: cap al endpoint A (0) para poder responder */
    grant_endpoint_cap(0, CAP_RIGHT_WRITE | CAP_RIGHT_READ);

    uart_puts("[B: cap a 0 concedida]");

    for (;;) {
        ipc_message_t in;
        if (ipc_recv(&in) == 0) {
            uart_puts("[B: msg recibido");

            if (in.has_cap) {
                uart_puts(" con cap adjunta]");

                /* Verificar que la cap recibida designa el objeto 5 */
                int idx = cap_lookup(5, CAP_RIGHT_READ);
                if (idx >= 0) {
                    uart_puts("[B: cap al objeto 5 recibida OK]");
                } else {
                    uart_puts("[B: ERROR cap al objeto 5 no está]");
                }

                /* Verificar que NO se propagó GRANT */
                int idx_grant = cap_lookup(5, CAP_RIGHT_GRANT);
                if (idx_grant < 0) {
                    uart_puts("[B: GRANT NO propagado (correcto)]");
                } else {
                    uart_puts("[B: ERROR GRANT propagado]");
                }
            } else {
                uart_puts(" sin cap]");
            }
        }

        task_yield();
    }
}

void kmain(void) {
    uart_init();
    uart_puts("OMEGA kernel v9 (cap passing)\n");
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
