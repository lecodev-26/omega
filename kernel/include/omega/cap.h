/*
 * OMEGA — Kernel
 * cap.h — Capabilities (object capabilities básicas)
 *
 * Este es un PROTOTIPO. Las capabilities designan endpoints IPC.
 * No hay paso de capabilities en mensajes todavía.
 */

#ifndef OMEGA_CAP_H
#define OMEGA_CAP_H

#include <stdint.h>

#define CAP_MAX_PER_TASK  16   /* capabilities por tarea */
#define CAP_RIGHT_NONE    0u
#define CAP_RIGHT_READ    (1u << 0)
#define CAP_RIGHT_WRITE   (1u << 1)
#define CAP_RIGHT_GRANT   (1u << 2)
#define CAP_RIGHT_REVOKE  (1u << 3)

/*
 * Una capability designa un objeto (endpoint) con derechos concretos.
 *
 * IMPORTANTE: alinear a 8 bytes para evitar Alignment faults.
 */
typedef struct {
    uint64_t object_id;    /* ID del objeto designado (endpoint) */
    uint64_t rights;       /* bitmask de derechos */
    uint64_t generation;   /* contador de generación (para revocación) */
    uint64_t valid;        /* 1 si es válida, 0 si fue revocada */
} __attribute__((aligned(8))) capability_t;

/*
 * Inicializa el subsistema de capabilities.
 */
void cap_init(void);

/*
 * Inicializa la tabla de capabilities de una tarea.
 * Llamado por task_create().
 */
void cap_table_init(uint32_t task_idx);

/*
 * Añade una capability a la tabla de la tarea actual.
 * Devuelve el índice en la tabla o -1 si no hay espacio.
 */
int cap_add(capability_t *cap);

/*
 * Busca una capability en la tabla de la tarea actual que designe
 * el objeto dado con al menos los derechos pedidos.
 * Devuelve el índice o -1 si no la encuentra.
 */
int cap_lookup(uint64_t object_id, uint64_t required_rights);

/*
 * Deriva una capability hija con menos derechos.
 * Devuelve el índice de la hija o -1 si los derechos exceden los de la madre.
 */
int cap_derive(int parent_idx, uint64_t subset_rights);

/*
 * Revoca una capability (por índice).
 */
void cap_revoke(int idx);

/*
 * Devuelve el número de capabilities válidas en la tarea actual.
 */
int cap_count(void);

#endif /* OMEGA_CAP_H */

/*
 * Devuelve un puntero a la capability en la tabla de la tarea actual.
 * Devuelve NULL si el índice no es válido.
 */
capability_t *cap_get(int idx);
