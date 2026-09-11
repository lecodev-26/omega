/*
 * OMEGA — Host prototype
 * capability.h — Capability conceptual
 *
 * Este es un PROTOTIPO CONCEPTUAL. No es código de kernel.
 *
 * En este prototipo, una capability es un token con:
 *   - un id de objeto
 *   - un conjunto de derechos (bitmask)
 *   - un nivel de derivación (para revocación)
 *
 * No hay aislamiento real. No hay kernel que valide.
 * El objetivo es validar el MODELO.
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
    uint32_t rights;       /* Bitmask de derechos */
    uint32_t generation;   /* Contador de generación para revocación */
    uint32_t parent_id;    /* Id de la capability padre (0 si raíz) */
    int      valid;        /* 1 si es válida, 0 si fue revocada */
} omega_capability_t;

/*
 * Inicializa una capability raíz.
 * Devuelve 0 en éxito, -1 en error.
 */
int omega_capability_init(omega_capability_t *cap,
                          uint32_t object_id,
                          uint32_t rights);

/*
 * Deriva una capability hija con un subconjunto de derechos.
 * La hija no puede tener más derechos que la madre.
 * Devuelve 0 en éxito, -1 si los derechos exceden los de la madre
 * o si la madre no es válida.
 */
int omega_capability_derive(const omega_capability_t *parent,
                            omega_capability_t *child,
                            uint32_t subset_rights);

/*
 * Revoca una capability.
 */
void omega_capability_revoke(omega_capability_t *cap);

/*
 * Comprueba si una capability tiene un derecho.
 * Devuelve 1 si lo tiene, 0 si no.
 */
int omega_capability_has(const omega_capability_t *cap, uint32_t right);

#endif /* OMEGA_CAPABILITY_H */
