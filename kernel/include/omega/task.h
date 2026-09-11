/*
 * OMEGA — Kernel
 * task.h — Tareas y scheduler cooperativo
 */

#ifndef OMEGA_TASK_H
#define OMEGA_TASK_H

#include <stdint.h>

#define TASK_NAME_MAX   32
#define TASK_STACK_SIZE 4096
#define MAX_TASKS       8

typedef enum {
    TASK_STATE_UNUSED = 0,
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_FINISHED
} task_state_t;

typedef struct {
    uint64_t x19, x20, x21, x22, x23, x24, x25, x26, x27, x28;
    uint64_t x29;   /* frame pointer */
    uint64_t x30;   /* link register */
    uint64_t sp;
    uint64_t pc;
} task_context_t;

/*
 * Estructura de una tarea.
 *
 * IMPORTANTE: los campos de 64 bits van primero para garantizar
 * alineación natural a 8 bytes. Si mezclamos ints y uint64_t sin
 * cuidado, el compilador puede generar accesos desalineados que
 * fallan con Alignment fault en aarch64.
 *
 * El array g_tasks también debe estar alineado a 16 bytes.
 */
typedef struct {
    uint64_t       idx;         /* índice en el array (como 64-bit para alineación) */
    uint64_t       yields;      /* contador de yields */
    task_context_t context;     /* contexto de la tarea */
    void         (*entry)(void);/* puntero a la función de entrada */
    task_state_t   state;       /* estado de la tarea */
    char           name[TASK_NAME_MAX]; /* nombre (32 bytes) */
    uint8_t        stack[TASK_STACK_SIZE]; /* stack de la tarea */
} __attribute__((aligned(16))) task_t;

void task_init(void);
int  task_create(const char *name, void (*entry)(void));
void task_yield(void);
int  task_count(void);
task_t *task_current(void);
task_t *task_get(int idx);

#endif /* OMEGA_TASK_H */
