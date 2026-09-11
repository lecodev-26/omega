/*
 * OMEGA — Host prototype
 * capability.c
 */

#include "omega/capability.h"
#include <string.h>

static uint32_t g_next_generation = 1;

int omega_capability_init(omega_capability_t *cap,
                          uint32_t object_id,
                          uint32_t rights) {
    if (cap == NULL) return -1;
    memset(cap, 0, sizeof(*cap));
    cap->object_id = object_id;
    cap->rights = rights;
    cap->generation = g_next_generation++;
    cap->parent_id = 0;
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
    child->rights = subset_rights;
    child->generation = g_next_generation++;
    child->parent_id = parent->object_id;
    child->valid = 1;
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
