/*
 * OMEGA — Kernel
 * task.h — Tareas y scheduler cooperativo (13a)
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

/*
 * Contexto de una tarea cooperativa.
 *
 * Guardamos solo lo que es callee-saved en la ABI AAPCS64:
 *   x19-x28, x29 (FP), x30 (LR), SP, PC
 *
 * Nota: PC puede considerarse la dirección de retorno guardada
 * en el stack cuando la función se llamó. Aquí lo guardamos
 * explícitamente en el contexto.
 */
typedef struct {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;   /* frame pointer */
    uint64_t x30;   /* link register */
    uint64_t sp;    /* stack pointer */
    uint64_t pc;    /* dirección de retorno */
} task_context_t;

typedef struct task {
    char           name[TASK_NAME_MAX];
    task_state_t   state;
    task_context_t context;
    uint8_t        stack[TASK_STACK_SIZE];
    void         (*entry)(void);
    uint64_t       yields;
} task_t;

void task_init(void);
int  task_create(const char *name, void (*entry)(void));
void task_yield(void);
int  task_count(void);
task_t *task_current(void);
task_t *task_get(int idx);

#endif /* OMEGA_TASK_H */
