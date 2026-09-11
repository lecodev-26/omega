/*
 * OMEGA — Host prototype
 * endpoint.h — Endpoint conceptual de IPC
 *
 * Este es un PROTOTIPO CONCEPTUAL. No es código de kernel.
 *
 * En este prototipo, un endpoint es una cola de mensajes en memoria.
 * No hay kernel, no hay hilos reales, no hay sincronización real.
 * El objetivo es validar el MODELO antes de implementarlo en kernel.
 */

#ifndef OMEGA_ENDPOINT_H
#define OMEGA_ENDPOINT_H

#include <stddef.h>
#include <stdint.h>
#include "message.h"

/* Capacidad máxima de la cola de un endpoint */
#define OMEGA_ENDPOINT_QUEUE_SIZE 16

typedef struct {
    omega_message_t queue[OMEGA_ENDPOINT_QUEUE_SIZE];
    size_t head;
    size_t tail;
    size_t count;
    int    valid;
} omega_endpoint_t;

/*
 * Inicializa un endpoint vacío.
 * Devuelve 0 en éxito, -1 en error.
 */
int omega_endpoint_init(omega_endpoint_t *ep);

/*
 * Envía un mensaje al endpoint.
 * Devuelve 0 en éxito, -1 si la cola está llena o el endpoint no es válido.
 */
int omega_endpoint_send(omega_endpoint_t *ep, const omega_message_t *msg);

/*
 * Recibe un mensaje del endpoint.
 * Devuelve 0 en éxito, -1 si la cola está vacía o el endpoint no es válido.
 */
int omega_endpoint_recv(omega_endpoint_t *ep, omega_message_t *out);

/*
 * Devuelve el número de mensajes pendientes.
 */
size_t omega_endpoint_count(const omega_endpoint_t *ep);

#endif /* OMEGA_ENDPOINT_H */
