/*
 * OMEGA — Host prototype
 * endpoint.c
 */

#include "omega/endpoint.h"
#include <string.h>

int omega_endpoint_init(omega_endpoint_t *ep) {
    if (ep == NULL) return -1;
    memset(ep, 0, sizeof(*ep));
    ep->valid = 1;
    return 0;
}

int omega_endpoint_send(omega_endpoint_t *ep, const omega_message_t *msg) {
    if (ep == NULL || msg == NULL || !ep->valid) return -1;
    if (ep->count >= OMEGA_ENDPOINT_QUEUE_SIZE) return -1;

    ep->queue[ep->tail] = *msg;
    ep->tail = (ep->tail + 1) % OMEGA_ENDPOINT_QUEUE_SIZE;
    ep->count++;
    return 0;
}

int omega_endpoint_recv(omega_endpoint_t *ep, omega_message_t *out) {
    if (ep == NULL || out == NULL || !ep->valid) return -1;
    if (ep->count == 0) return -1;

    *out = ep->queue[ep->head];
    ep->head = (ep->head + 1) % OMEGA_ENDPOINT_QUEUE_SIZE;
    ep->count--;
    return 0;
}

size_t omega_endpoint_count(const omega_endpoint_t *ep) {
    if (ep == NULL || !ep->valid) return 0;
    return ep->count;
}
