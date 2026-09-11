/*
 * OMEGA — Host prototype
 * message.h — Mensaje básico de IPC
 *
 * Este es un PROTOTIPO CONCEPTUAL. No es código de kernel.
 */

#ifndef OMEGA_MESSAGE_H
#define OMEGA_MESSAGE_H

#include <stddef.h>
#include <stdint.h>

/* Tamaños soportados por el workload del Piloto 1 */
#define OMEGA_MSG_SIZE_8     8
#define OMEGA_MSG_SIZE_64    64
#define OMEGA_MSG_SIZE_512   512
#define OMEGA_MSG_SIZE_4096  4096

/* Tamaño máximo soportado */
#define OMEGA_MSG_MAX_SIZE   OMEGA_MSG_SIZE_4096

/*
 * Estructura de un mensaje.
 * El payload es de tamaño fijo para simplificar el prototipo.
 */
typedef struct {
    uint32_t length;                     /* Bytes válidos en payload */
    uint32_t id;                         /* Identificador de transacción */
    uint8_t  payload[OMEGA_MSG_MAX_SIZE];
} omega_message_t;

/*
 * Inicializa un mensaje con longitud y id.
 * Devuelve 0 en éxito, -1 en error.
 */
int omega_message_init(omega_message_t *msg, uint32_t id, uint32_t length);

/*
 * Rellena el payload con un patrón determinista (id + posición).
 * Útil para verificar integridad.
 */
void omega_message_fill_pattern(omega_message_t *msg);

/*
 * Verifica que el payload coincide con el patrón esperado.
 * Devuelve 0 si coincide, -1 si no.
 */
int omega_message_verify_pattern(const omega_message_t *msg);

#endif /* OMEGA_MESSAGE_H */
