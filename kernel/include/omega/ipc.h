/*
 * OMEGA — Kernel
 * ipc.h — Comunicación entre tareas con capabilities y blocking
 */

#ifndef OMEGA_IPC_H
#define OMEGA_IPC_H

#include <stdint.h>
#include "omega/cap.h"

#define IPC_MAX_ENDPOINTS   8
#define IPC_QUEUE_SIZE      16
#define IPC_PAYLOAD_MAX     64

#define IPC_MSG_PING        1
#define IPC_MSG_PONG        2
#define IPC_MSG_DATA        3

/*
 * Mensaje de IPC.
 *
 * Puede llevar adjunta UNA capability (at_idx_in_sender = índice en la
 * tabla del emisor; al recibir, se añade a la tabla del receptor).
 *
 * Atributos de alineación: alineamos a 8 bytes para evitar Alignment faults.
 */
typedef struct {
    uint32_t sender;
    uint32_t type;
    uint32_t length;
    uint32_t has_cap;      /* 1 si lleva capability adjunta, 0 si no */
    capability_t attached_cap;  /* capability adjunta (si has_cap == 1) */
    uint8_t  payload[IPC_PAYLOAD_MAX];
} __attribute__((aligned(8))) ipc_message_t;

void ipc_init(void);
int  ipc_send(uint32_t target_endpoint, const ipc_message_t *msg);
int  ipc_send_with_cap(uint32_t target_endpoint,
                       const ipc_message_t *msg,
                       int sender_cap_idx);
int  ipc_recv(ipc_message_t *out);
int  ipc_pending(uint32_t endpoint);
void ipc_register_endpoint(uint32_t idx);

#endif /* OMEGA_IPC_H */
