/*
 * OMEGA — Host prototype
 * capability.c (v2)
 */

#include "omega/capability.h"
#include <string.h>

static uint32_t g_next_generation = 1;

int omega_capability_init(omega_capability_t *cap,
                          uint32_t object_id,
                          uint32_t owner_id,
                          uint32_t rights) {
    if (cap == NULL) return -1;
    memset(cap, 0, sizeof(*cap));
    cap->object_id = object_id;
    cap->owner_id = owner_id;
    cap->rights = rights;
    cap->generation = g_next_generation++;
    cap->parent_owner = 0;
    cap->valid = 1;
    return 0;
}

int omega_capability_derive(const omega_capability_t *parent,
                            omega_capability_t *child,
                            uint32_t subset_rights) {
    if (parent == NULL || child == NULL) return -1;
    if (!parent->valid) return -1;

    /* La hija no puede tener más derechos que la madre */
    if ((subset_rights & ~parent->rights) != 0) return -1;

    memset(child, 0, sizeof(*child));
    child->object_id = parent->object_id;
    child->owner_id = parent->owner_id;
    child->rights = subset_rights;
    child->generation = g_next_generation++;
    child->parent_owner = parent->owner_id;
    child->valid = 1;
    return 0;
}

int omega_capability_delegate(const omega_capability_t *source,
                              uint32_t new_owner_id,
                              omega_capability_t *delegated) {
    if (source == NULL || delegated == NULL) return -1;
    if (!source->valid) return -1;
    if (!omega_capability_has(source, OMEGA_CAP_RIGHT_GRANT)) return -1;

    /* El delegado es una copia con owner diferente */
    memset(delegated, 0, sizeof(*delegated));
    delegated->object_id = source->object_id;
    delegated->owner_id = new_owner_id;
    delegated->rights = source->rights & ~OMEGA_CAP_RIGHT_GRANT; /* No se delega GRANT */
    delegated->generation = g_next_generation++;
    delegated->parent_owner = source->owner_id;
    delegated->valid = 1;
    return 0;
}

void omega_capability_revoke(omega_capability_t *cap) {
    if (cap == NULL) return;
    cap->valid = 0;
    cap->rights = OMEGA_CAP_RIGHT_NONE;
}

int omega_capability_has(const omega_capability_t *cap, uint32_t right) {
    if (cap == NULL || !cap->valid) return 0;
    return (cap->rights & right) != 0;
}

int omega_capability_equal(const omega_capability_t *a,
                           const omega_capability_t *b) {
    if (a == NULL || b == NULL) return 0;
    return a->object_id == b->object_id
        && a->owner_id == b->owner_id
        && a->rights == b->rights
        && a->valid == b->valid;
}
