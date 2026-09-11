/*
 * OMEGA — Host prototype
 * service.c (v2)
 */

#include "omega/service.h"
#include <string.h>

static uint32_t g_time_counter = 0;

int omega_service_init(omega_service_t *svc,
                       uint32_t service_id,
                       const char *name,
                       omega_service_handler_t handler) {
    if (svc == NULL || name == NULL || handler == NULL) return -1;

    memset(svc, 0, sizeof(*svc));
    svc->service_id = service_id;
    strncpy(svc->name, name, OMEGA_SERVICE_NAME_MAX - 1);
    svc->name[OMEGA_SERVICE_NAME_MAX - 1] = '\0';
    svc->handler = handler;

    if (omega_endpoint_init(&svc->inbox) != 0) return -1;

    svc->valid = 1;
    return 0;
}

int omega_service_step(omega_service_t *svc, omega_message_t *out) {
    if (svc == NULL || !svc->valid || out == NULL) return -1;

    omega_message_t request;
    if (omega_endpoint_recv(&svc->inbox, &request) != 0) return -1;

    if (omega_message_init(out, request.id, request.length) != 0) return -1;

    if (svc->handler(&request, out) != 0) return -1;

    return 0;
}

int omega_service_handler_echo(const omega_message_t *req,
                               omega_message_t *resp) {
    if (req == NULL || resp == NULL) return -1;
    if (req->length > OMEGA_MSG_MAX_SIZE) return -1;

    memcpy(resp->payload, req->payload, req->length);
    resp->length = req->length;
    resp->id = req->id;
    return 0;
}

int omega_service_handler_time(const omega_message_t *req,
                               omega_message_t *resp) {
    if (req == NULL || resp == NULL) return -1;

    g_time_counter++;

    uint32_t n = (req->length < sizeof(uint32_t)) ? req->length : sizeof(uint32_t);
    memcpy(resp->payload, &g_time_counter, n);
    resp->length = n;
    resp->id = req->id;
    return 0;
}

const char *omega_service_name_of(uint32_t service_id) {
    switch (service_id) {
        case OMEGA_SERVICE_ECHO: return "echo";
        case OMEGA_SERVICE_TIME: return "time";
        default: return NULL;
    }
}
