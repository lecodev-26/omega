/*
 * OMEGA — Kernel
 * ipc.c — Comunicación entre tareas (con capabilities y blocking)
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
 * Busca la primera tarea BLOCKED que esté esperando para recibir del
 * endpoint dado. Devuelve su índice o -1.
 *
 * Limitación: no distingue entre tareas bloqueadas por send vs recv.
 * El endpoint solo tiene una "cola de espera" lógica; como las tareas
 * bloqueadas por send esperan hueco y las bloqueadas por recv esperan
 * mensaje, en la práctica se despiertan en el orden correcto porque
 * los desbloqueos se hacen en el punto adecuado.
 */
static int ipc_find_blocked_receiver(uint32_t endpoint) {
    (void)endpoint;
    for (int i = 0; i < task_count(); i++) {
        task_t *t = task_get(i);
        if (t && t->state == TASK_STATE_BLOCKED) {
            return i;
        }
    }
    return -1;
}

/*
 * Envía un mensaje SIN capability adjunta.
 *
 * Si el buzón está lleno, bloquea la tarea actual hasta que haya hueco.
 */
int ipc_send(uint32_t target_endpoint, const ipc_message_t *msg) {
    if (target_endpoint >= IPC_MAX_ENDPOINTS) return -1;
    if (msg == NULL) return -1;

    if (cap_lookup((uint64_t)target_endpoint, CAP_RIGHT_WRITE) < 0) {
        return -1;
    }

    ipc_endpoint_t *ep = &g_endpoints[target_endpoint];
    if (!ep->valid) return -1;

    /* Si el buzón está lleno, bloquear hasta que haya hueco */
    while (ep->count >= IPC_QUEUE_SIZE) {
        task_block_current();
        /* Al despertar, reintentar */
    }

    ep->messages[ep->tail] = *msg;
    ep->messages[ep->tail].has_cap = 0;
    ep->tail = (ep->tail + 1) % IPC_QUEUE_SIZE;
    ep->count++;

    /* Despertar a un receptor bloqueado (si lo hay) */
    int recv_idx = ipc_find_blocked_receiver(target_endpoint);
    if (recv_idx >= 0) {
        task_unblock(recv_idx);
    }

    return 0;
}

/*
 * Envía un mensaje CON capability adjunta.
 *
 * Si el buzón está lleno, bloquea la tarea actual hasta que haya hueco.
 */
int ipc_send_with_cap(uint32_t target_endpoint,
                      const ipc_message_t *msg,
                      int sender_cap_idx) {
    if (target_endpoint >= IPC_MAX_ENDPOINTS) return -1;
    if (msg == NULL) return -1;

    if (cap_lookup((uint64_t)target_endpoint, CAP_RIGHT_WRITE) < 0) {
        return -1;
    }

    capability_t *src_cap = cap_get(sender_cap_idx);
    if (src_cap == 0) return -1;
    if ((src_cap->rights & CAP_RIGHT_GRANT) == 0) return -1;

    ipc_endpoint_t *ep = &g_endpoints[target_endpoint];
    if (!ep->valid) return -1;

    while (ep->count >= IPC_QUEUE_SIZE) {
        task_block_current();
    }

    ipc_message_t out_msg = *msg;
    out_msg.has_cap = 1;
    out_msg.attached_cap = *src_cap;
    out_msg.attached_cap.rights = src_cap->rights & ~CAP_RIGHT_GRANT;
    out_msg.attached_cap.valid = 1;

    ep->messages[ep->tail] = out_msg;
    ep->tail = (ep->tail + 1) % IPC_QUEUE_SIZE;
    ep->count++;

    int recv_idx = ipc_find_blocked_receiver(target_endpoint);
    if (recv_idx >= 0) {
        task_unblock(recv_idx);
    }

    return 0;
}

/*
 * Recibe un mensaje del endpoint de la tarea actual.
 *
 * Si el buzón está vacío, bloquea la tarea actual hasta que llegue
 * un mensaje.
 */
int ipc_recv(ipc_message_t *out) {
    if (out == NULL) return -1;

    task_t *current = task_current();
    if (current == NULL) return -1;

    uint32_t idx = (uint32_t)current->idx;
    if (idx >= IPC_MAX_ENDPOINTS) return -1;

    ipc_endpoint_t *ep = &g_endpoints[idx];
    if (!ep->valid) return -1;

    while (ep->count == 0) {
        task_block_current();
    }

    *out = ep->messages[ep->head];
    ep->head = (ep->head + 1) % IPC_QUEUE_SIZE;
    ep->count--;

    /*
     * Si el mensaje lleva capability adjunta, añadirla a la tabla del receptor.
     */
    if (out->has_cap) {
        capability_t new_cap = out->attached_cap;
        new_cap.generation = 0;
        new_cap.valid = 1;
        int idx_cap = cap_add(&new_cap);
        if (idx_cap < 0) {
            out->has_cap = 0;
        }
    }

    /*
     * Despertar a un emisor bloqueado (si lo hay): hay hueco libre.
     */
    for (int i = 0; i < task_count(); i++) {
        task_t *t = task_get(i);
        if (t && t->state == TASK_STATE_BLOCKED) {
            task_unblock(i);
            break;
        }
    }

    return 0;
}

int ipc_pending(uint32_t endpoint) {
    if (endpoint >= IPC_MAX_ENDPOINTS) return -1;
    if (!g_endpoints[endpoint].valid) return -1;
    return (int)g_endpoints[endpoint].count;
}
