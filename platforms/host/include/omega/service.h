/*
 * OMEGA — Host prototype
 * service.h — Servicio conceptual (v2)
 *
 * Un servicio es una unidad que:
 *   - recibe mensajes en un endpoint
 *   - procesa cada mensaje
 *   - produce una respuesta
 *
 * Este es un PROTOTIPO CONCEPTUAL. No hay scheduling real.
 */

#ifndef OMEGA_SERVICE_H
#define OMEGA_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include "message.h"
#include "endpoint.h"

#define OMEGA_SERVICE_NAME_MAX 64

#define OMEGA_SERVICE_ECHO  1u
#define OMEGA_SERVICE_TIME  2u

typedef int (*omega_service_handler_t)(const omega_message_t *request,
                                       omega_message_t *response);

typedef struct {
    uint32_t service_id;
    char     name[OMEGA_SERVICE_NAME_MAX];
    omega_endpoint_t inbox;
    omega_service_handler_t handler;
    int      valid;
} omega_service_t;

/*
 * Inicializa un servicio con un nombre y un handler.
 */
int omega_service_init(omega_service_t *svc,
                       uint32_t service_id,
                       const char *name,
                       omega_service_handler_t handler);

/*
 * Procesa el siguiente mensaje pendiente del inbox.
 * Si hay un mensaje, invoca el handler y escribe la respuesta en *out.
 * Devuelve:
 *   0  si procesó un mensaje correctamente
 *  -1  si no había mensaje o hubo error
 */
int omega_service_step(omega_service_t *svc, omega_message_t *out);

/*
 * Handlers predefinidos.
 */
int omega_service_handler_echo(const omega_message_t *req,
                               omega_message_t *resp);

int omega_service_handler_time(const omega_message_t *req,
                               omega_message_t *resp);

/*
 * Devuelve un nombre estático para un service_id conocido.
 * Devuelve NULL si no lo conoce.
 */
const char *omega_service_name_of(uint32_t service_id);

#endif /* OMEGA_SERVICE_H */
