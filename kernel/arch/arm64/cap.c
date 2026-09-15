/*
 * OMEGA — Kernel
 * cap.c — Capabilities (object capabilities básicas)
 */

#include <stddef.h>
#include "omega/cap.h"
#include "omega/task.h"

#define MAX_TASKS_FOR_CAP 8

typedef struct {
    capability_t caps[CAP_MAX_PER_TASK];
    int          count;
} cap_table_t;

static cap_table_t g_cap_tables[MAX_TASKS_FOR_CAP] __attribute__((aligned(16)));

static uint64_t g_next_generation = 1;

void cap_init(void) {
    for (int i = 0; i < MAX_TASKS_FOR_CAP; i++) {
        g_cap_tables[i].count = 0;
        for (int j = 0; j < CAP_MAX_PER_TASK; j++) {
            g_cap_tables[i].caps[j].valid = 0;
        }
    }
}

void cap_table_init(uint32_t task_idx) {
    if (task_idx >= MAX_TASKS_FOR_CAP) return;
    g_cap_tables[task_idx].count = 0;
    for (int j = 0; j < CAP_MAX_PER_TASK; j++) {
        g_cap_tables[task_idx].caps[j].valid = 0;
    }
}

static int cap_find_free(cap_table_t *tbl) {
    for (int i = 0; i < CAP_MAX_PER_TASK; i++) {
        if (!tbl->caps[i].valid) return i;
    }
    return -1;
}

int cap_add(capability_t *cap) {
    task_t *current = task_current();
    if (current == NULL) return -1;
    return cap_add_to((uint32_t)current->idx, cap);
}

/*
 * Añade una capability a la tabla de la tarea indicada.
 * Función interna del kernel. No valida autoridad: se usa durante
 * la inicialización del sistema.
 */
int cap_add_to(uint32_t task_idx, capability_t *cap) {
    if (task_idx >= MAX_TASKS_FOR_CAP) return -1;
    if (cap == NULL) return -1;

    cap_table_t *tbl = &g_cap_tables[task_idx];
    int free_idx = cap_find_free(tbl);
    if (free_idx < 0) return -1;

    tbl->caps[free_idx] = *cap;
    tbl->caps[free_idx].generation = g_next_generation++;
    tbl->caps[free_idx].valid = 1;
    tbl->count++;
    return free_idx;
}

int cap_lookup(uint64_t object_id, uint64_t required_rights) {
    task_t *current = task_current();
    if (current == NULL) return -1;

    uint32_t idx = (uint32_t)current->idx;
    if (idx >= MAX_TASKS_FOR_CAP) return -1;

    cap_table_t *tbl = &g_cap_tables[idx];
    for (int i = 0; i < CAP_MAX_PER_TASK; i++) {
        capability_t *c = &tbl->caps[i];
        if (!c->valid) continue;
        if (c->object_id != object_id) continue;
        if ((c->rights & required_rights) != required_rights) continue;
        return i;
    }
    return -1;
}

int cap_derive(int parent_idx, uint64_t subset_rights) {
    task_t *current = task_current();
    if (current == NULL) return -1;

    uint32_t idx = (uint32_t)current->idx;
    if (idx >= MAX_TASKS_FOR_CAP) return -1;

    cap_table_t *tbl = &g_cap_tables[idx];
    if (parent_idx < 0 || parent_idx >= CAP_MAX_PER_TASK) return -1;

    capability_t *parent = &tbl->caps[parent_idx];
    if (!parent->valid) return -1;

    if ((subset_rights & ~parent->rights) != 0) return -1;

    int free_idx = cap_find_free(tbl);
    if (free_idx < 0) return -1;

    tbl->caps[free_idx].object_id  = parent->object_id;
    tbl->caps[free_idx].rights     = subset_rights;
    tbl->caps[free_idx].generation = g_next_generation++;
    tbl->caps[free_idx].valid      = 1;
    tbl->count++;
    return free_idx;
}

void cap_revoke(int idx) {
    task_t *current = task_current();
    if (current == NULL) return;

    uint32_t task_idx = (uint32_t)current->idx;
    if (task_idx >= MAX_TASKS_FOR_CAP) return;
    if (idx < 0 || idx >= CAP_MAX_PER_TASK) return;

    cap_table_t *tbl = &g_cap_tables[task_idx];
    if (tbl->caps[idx].valid) {
        tbl->caps[idx].valid = 0;
        tbl->caps[idx].rights = CAP_RIGHT_NONE;
        if (tbl->count > 0) tbl->count--;
    }
}

int cap_count(void) {
    task_t *current = task_current();
    if (current == NULL) return -1;

    uint32_t idx = (uint32_t)current->idx;
    if (idx >= MAX_TASKS_FOR_CAP) return -1;
    return g_cap_tables[idx].count;
}

capability_t *cap_get(int idx) {
    task_t *current = task_current();
    if (current == NULL) return 0;

    uint32_t task_idx = (uint32_t)current->idx;
    if (task_idx >= MAX_TASKS_FOR_CAP) return 0;
    if (idx < 0 || idx >= CAP_MAX_PER_TASK) return 0;

    cap_table_t *tbl = &g_cap_tables[task_idx];
    if (!tbl->caps[idx].valid) return 0;
    return &tbl->caps[idx];
}
