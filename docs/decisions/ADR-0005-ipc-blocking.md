# ADR-0005: IPC blocking

## Contexto

El kernel prototype tenía IPC no bloqueante: `ipc_send` fallaba si el
buzón estaba lleno, `ipc_recv` fallaba si estaba vacío. Eso obligaba
a las tareas a hacer polling, lo cual no es un IPC real.

Con la llegada de la preemption (ADR-0004), se puede implementar
blocking de forma eficiente: la tarea se bloquea, el scheduler elige
otra, y cuando el recurso está disponible, la tarea se despierta.

## Problema

¿Cómo implementar `send`/`recv` que esperen cuando el buzón está
lleno/vacío, sin busy-wait, aprovechando la preemption?

## Opciones

### Opción A — Mantener IPC no bloqueante

Dejar `ipc_send`/`ipc_recv` como estaban. Las tareas hacen polling.

**Ventajas:**
- Simple.
- Sin estado adicional.

**Desventajas:**
- No es un IPC real.
- Las tareas consumen CPU haciendo polling.
- No escala.

### Opción B — IPC blocking con lista de espera por endpoint

Añadir una lista de espera por endpoint (o dos: send y recv). Cuando
una tarea no puede completar la operación, se bloquea. Cuando el
recurso está disponible, se despierta a una tarea de la lista.

**Ventajas:**
- IPC real.
- Eficiente: la tarea bloqueada no consume CPU.
- Base para futuros mecanismos de sincronización.

**Desventajas:**
- Más complejo.
- Requiere estado adicional por endpoint.
- Requiere secciones críticas para evitar carreras.

### Opción C — IPC asíncrono con notificaciones

`send` nunca bloquea, devuelve un token. `recv` bloquea. Notificaciones
cuando llega el mensaje.

**Ventajas:**
- `send` no bloquea.

**Desventajas:**
- Más complejo todavía.
- No es lo que queremos ahora.

## Evidencia

- Los microkernels modernos (seL4, L4) usan IPC blocking.
- QNX usa IPC blocking.
- El host prototype v3 ya tenía blocking conceptual.
- La preemption (ADR-0004) permite implementarlo eficientemente.

## Decisión

**Implementar IPC blocking** con las siguientes decisiones:

### 1. Estado BLOCKED por tarea

Añadir `TASK_STATE_BLOCKED` a `task_state_t`. Una tarea BLOCKED no es
elegible por el scheduler.

### 2. `task_block_current()` y `task_unblock(idx)`

- `task_block_current()`: pone la tarea actual en BLOCKED y cede el
  control (`task_yield()`). Retorna cuando la tarea es desbloqueada.
- `task_unblock(idx)`: pone la tarea `idx` en READY (si estaba BLOCKED).

### 3. Lista de espera simplificada

Por ahora, no hay una lista de espera por endpoint. En su lugar, cuando
se envía un mensaje, se despierta a la **primera** tarea BLOCKED. Cuando
se recibe un mensaje, se despierta a la **primera** tarea BLOCKED.

**Limitación:** no distingue entre tareas bloqueadas por send vs recv.
Funciona para el caso simple (una tarea espera hueco, otra espera
mensaje). Se mejorará en un ADR posterior.

### 4. Secciones críticas con IRQs deshabilitadas

`ipc_send`/`ipc_recv` acceden a las colas dentro de secciones críticas
con IRQs deshabilitadas. Esto evita que el timer interrumpa entre
`ep->count++` y `task_unblock()`, lo cual causaba deadlock.

**Sintaxis:**
```c
uint64_t daif = uart_lock();   /* deshabilita IRQs, guarda DAIF */
/* ... acceso a la cola ... */
uart_unlock(daif);             /* restaura DAIF */
```

5. task_yield atómico

task_yield deshabilita IRQs alrededor del cambio de contexto
cooperativo, y las restaura al retornar. Esto evita que el timer
interrumpa en medio del cambio y cause deadlock.

Excepción: en el arranque inicial (prev < 0), no se deshabilitan
IRQs, porque la tarea nueva debe arrancar con IRQs habilitadas.

6. Sincronización de la UART

Se reemplaza el spinlock de la UART por daif save/restore. En un
sistema uniprocesador, deshabilitar IRQs mientras se imprime es
suficiente. No hace falta spinlock.

Consecuencias

· El kernel prototype tiene IPC blocking funcional.
· Verificado en QEMU virt aarch64: ping-pong sin task_yield()
  explícito, con preemption, sin deadlock.
· La lista de espera es simplificada (no distingue send vs recv).
· No hay timeout en las operaciones de IPC.
· No hay prioridades en el scheduler (todas las tareas son iguales).

Estado

Aceptado. Implementado y verificado.

Ver también

· docs/decisions/ADR-0004-preemption-timer.md — Preemption.
· kernel/arch/arm64/ipc.c — Implementación.
· kernel/arch/arm64/task.c — task_block_current, task_unblock.
· kernel/arch/arm64/uart.c — Sincronización.
