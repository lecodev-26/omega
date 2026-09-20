# ADR-0003: Modelo de scheduler

## Contexto

El kernel prototype incluye un scheduler de tareas. Inicialmente era
cooperativo. Ahora es preemptivo con timer.

Este ADR documenta las decisiones tomadas.

## Decisiones tomadas

### 1. Tareas como unidades de ejecución

Cada tarea es una función C con stack de 4 KB. Máximo 8 tareas.

### 2. Multitarea preemptiva

El timer del sistema interrumpe cada 100 ms. El stub de IRQ decide si
cambiar de tarea. El cambio de contexto se hace desde IRQ.

**Ver:** ADR-0004 — Preemption con timer.

### 3. Multitarea cooperativa

Las tareas también pueden ceder el control con `task_yield()`.

### 4. Contexto extendido

`task_context_t` incluye x0-x30 + SP + PC + SPSR. Necesario para
reanudar una tarea interrumpida por IRQ.

### 5. Round-robin como política

El scheduler es round-robin simple. Elige la siguiente tarea READY/RUNNING
en orden circular.

**Limitación:** No hay prioridades ni deadlines.

### 6. Trampoline por tarea

Toda tarea nueva arranca desde `task_trampoline`, que recibe el puntero
a la task en x19.

### 7. `task_yield` atómico

`task_yield` deshabilita IRQs alrededor del cambio cooperativo y las
restaura al retornar. Evita que el timer interrumpa en medio del cambio
y cause deadlock.

### 8. Estado BLOCKED

Las tareas pueden estar BLOCKED (esperando un recurso). El scheduler las
ignora. `task_block_current()` y `task_unblock(idx)` gestionan el bloqueo.

**Ver:** ADR-0005 — IPC blocking.

## Consecuencias

- El kernel prototype tiene multitarea preemptiva funcional.
- No hay prioridades, deadlines ni SMP.
- La política es round-robin simple.
- El quantum es fijo (100 ms), no configurable.

## Estado

**Aceptado.**

## Ver también

- `docs/decisions/ADR-0004-preemption-timer.md` — Preemption con timer.
- `docs/decisions/ADR-0005-ipc-blocking.md` — IPC blocking.
- `kernel/arch/arm64/task.c` — Implementación.
- `kernel/arch/arm64/switch.S` — Cambio de contexto.
