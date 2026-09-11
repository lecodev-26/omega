/*
 * OMEGA — Kernel
 * ipc.c — Comunicación entre tareas (con capabilities)
 */

#include <stddef.h>
#include "omega/ipc.h"
#include "omega/task.h"
#include "omega/cap.h"

typedef struct {
    ipc_message_t messages[IPC_QUEUE_SIZE];
    uint32_t      head;
    uint32_t      tail;
    uint32_t      count;
    uint32_t      valid;
} ipc_endpoint_t;

static ipc_endpoint_t g_endpoints[IPC_MAX_ENDPOINTS] __attribute__((aligned(16)));

void ipc_init(void) {
    for (int i = 0; i < IPC_MAX_ENDPOINTS; i++) {
        g_endpoints[i].head  = 0;
        g_endpoints[i].tail  = 0;
        g_endpoints[i].count = 0;
        g_endpoints[i].valid = 0;
    }
}

void ipc_register_endpoint(uint32_t idx) {
    if (idx >= IPC_MAX_ENDPOINTS) return;
    g_endpoints[idx].head  = 0;
    g_endpoints[idx].tail  = 0;
    g_endpoints[idx].count = 0;
    g_endpoints[idx].valid = 1;
}

/*
 * Envía un mensaje al endpoint destino.
 *
 * REQUIERE que la tarea actual tenga una capability que designe
 * el endpoint destino con CAP_RIGHT_WRITE.
 */
int ipc_send(uint32_t target_endpoint, const ipc_message_t *msg) {
    if (target_endpoint >= IPC_MAX_ENDPOINTS) return -1;
    if (msg == NULL) return -1;

    /* Verificar capability */
    if (cap_lookup((uint64_t)target_endpoint, CAP_RIGHT_WRITE) < 0) {
        return -1;  /* Sin capability → denegado */
    }

    ipc_endpoint_t *ep = &g_endpoints[target_endpoint];
    if (!ep->valid) return -1;
    if (ep->count >= IPC_QUEUE_SIZE) return -1;

    ep->messages[ep->tail] = *msg;
    ep->tail = (ep->tail + 1) % IPC_QUEUE_SIZE;
    ep->count++;
    return 0;
}

int ipc_recv(ipc_message_t *out) {
    if (out == NULL) return -1;

    task_t *current = task_current();
    if (current == NULL) return -1;

    uint32_t idx = (uint32_t)current->idx;
    if (idx >= IPC_MAX_ENDPOINTS) return -1;

    ipc_endpoint_t *ep = &g_endpoints[idx];
    if (!ep->valid) return -1;
    if (ep->count == 0) return -1;

    *out = ep->messages[ep->head];
    ep->head = (ep->head + 1) % IPC_QUEUE_SIZE;
    ep->count--;
    return 0;
}

int ipc_pending(uint32_t endpoint) {
    if (endpoint >= IPC_MAX_ENDPOINTS) return -1;
    if (!g_endpoints[endpoint].valid) return -1;
    return (int)g_endpoints[endpoint].count;
}
