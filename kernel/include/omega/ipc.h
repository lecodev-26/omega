/*
 * OMEGA — Kernel
 * ipc.h — Comunicación entre tareas (IPC básico)
 *
 * Este es un PROTOTIPO. No hay capabilities, no hay paso de FDs,
 * no hay bloqueo si el buzón está lleno.
 */

#ifndef OMEGA_IPC_H
#define OMEGA_IPC_H

#include <stdint.h>

#define IPC_MAX_ENDPOINTS   8
#define IPC_QUEUE_SIZE      16
#define IPC_PAYLOAD_MAX     64

#define IPC_MSG_PING        1
#define IPC_MSG_PONG        2
#define IPC_MSG_DATA        3

/*
 * Mensaje de IPC.
 *
 * IMPORTANTE: alineamos a 8 bytes para evitar Alignment faults.
 * El compilador puede generar stores de 8 bytes (stur x8) si asume
 * que la struct está alineada a 8. Si no lo está, el store falla
 * en aarch64 con Alignment fault.
 */
typedef struct {
    uint32_t sender;
    uint32_t type;
    uint32_t length;
    uint32_t _reserved;   /* padding para alinear a 8 */
    uint8_t  payload[IPC_PAYLOAD_MAX];
} __attribute__((aligned(8))) ipc_message_t;

void ipc_init(void);
int  ipc_send(uint32_t target_endpoint, const ipc_message_t *msg);
int  ipc_recv(ipc_message_t *out);
int  ipc_pending(uint32_t endpoint);

/*
 * Registra un endpoint en el índice dado.
 * Llamado por task_create() cuando se crea una tarea.
 */
void ipc_register_endpoint(uint32_t idx);

#endif /* OMEGA_IPC_H */
