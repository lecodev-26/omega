/*
 * OMEGA — Host prototype
 * capability.h — Capability conceptual (v2)
 *
 * Este es un PROTOTIPO CONCEPTUAL. No es código de kernel.
 *
 * Una capability es un token con:
 *   - un id de objeto referenciado
 *   - un conjunto de derechos (bitmask)
 *   - un owner (quién la posee)
 *   - un nivel de derivación (para revocación)
 *   - un id de padre (para reconstruir el árbol de derivación)
 *
 * La delegación permite transferir una capability a otro owner.
 */

#ifndef OMEGA_CAPABILITY_H
#define OMEGA_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>

/* Derechos de una capability */
#define OMEGA_CAP_RIGHT_NONE    0u
#define OMEGA_CAP_RIGHT_READ    (1u << 0)
#define OMEGA_CAP_RIGHT_WRITE   (1u << 1)
#define OMEGA_CAP_RIGHT_EXEC    (1u << 2)
#define OMEGA_CAP_RIGHT_GRANT   (1u << 3)
#define OMEGA_CAP_RIGHT_REVOKE  (1u << 4)

typedef struct {
    uint32_t object_id;    /* Identificador del objeto referenciado */
    uint32_t owner_id;     /* Quién posee la capability */
    uint32_t rights;       /* Bitmask de derechos */
    uint32_t generation;   /* Contador de generación (para revocación) */
    uint32_t parent_owner; /* Owner del que se heredó (0 si raíz) */
    int      valid;        /* 1 si es válida, 0 si fue revocada */
} omega_capability_t;

/*
 * Inicializa una capability raíz.
 * Devuelve 0 en éxito, -1 en error.
 */
int omega_capability_init(omega_capability_t *cap,
                          uint32_t object_id,
                          uint32_t owner_id,
                          uint32_t rights);

/*
 * Deriva una capability hija con un subconjunto de derechos.
 * La hija no puede tener más derechos que la madre.
 * La hija pertenece al mismo owner.
 */
int omega_capability_derive(const omega_capability_t *parent,
                            omega_capability_t *child,
                            uint32_t subset_rights);

/*
 * Delega una capability a otro owner.
 * El owner original conserva la suya (copia).
 * Devuelve 0 en éxito, -1 si la capability no es válida
 * o no tiene el derecho GRANT.
 */
int omega_capability_delegate(const omega_capability_t *source,
                              uint32_t new_owner_id,
                              omega_capability_t *delegated);

/*
 * Revoca una capability.
 */
void omega_capability_revoke(omega_capability_t *cap);

/*
 * Comprueba si una capability tiene un derecho.
 * Devuelve 1 si lo tiene, 0 si no.
 */
int omega_capability_has(const omega_capability_t *cap, uint32_t right);

/*
 * Comprueba si dos capabilities son equivalentes
 * (mismo objeto, mismos derechos, mismos owners).
 */
int omega_capability_equal(const omega_capability_t *a,
                           const omega_capability_t *b);

#endif /* OMEGA_CAPABILITY_H */
