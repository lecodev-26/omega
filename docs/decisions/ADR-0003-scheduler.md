# ADR-0003: Modelo de scheduler

## Contexto

El kernel prototype incluye un scheduler de tareas. Inicialmente era
cooperativo: las tareas cedían el control con `task_yield()`. La
multitarea preemptiva (con el timer forzando el cambio) estaba
preparada pero no activada.

Este ADR documenta las decisiones tomadas.

## Decisiones tomadas

### 1. Tareas como unidades de ejecución

**Decisión:** Cada tarea es una función C con su propio stack (4 KB)
y contexto. Máximo 8 tareas concurrentes.

**Justificación:** Modelo simple que valida el mecanismo. Ampliable
en el futuro.

### 2. Multitarea cooperativa (fase inicial)

**Decisión:** Las tareas ceden el control explícitamente con
`task_yield()`.

**Justificación:** El cambio de contexto cooperativo es más simple
y no requiere modificar el handler de IRQ.

**Limitación:** Una tarea que no cede el control monopoliza la CPU.

**Estado:** Superado por la decisión 7 (preemption).

### 3. Contexto cooperativo: callee-saved

**Decisión:** El `context_switch` cooperativo guarda/restaura solo los
registros callee-saved (x19-x30) + SP + PC.

**Justificación:** El cambio de contexto se hace desde una llamada
a función normal. Los caller-saved (x0-x18) ya están guardados en el
stack del llamador.

**Estado:** Aceptado. Sigue vigente para `task_yield()`.

### 4. Contexto extendido

**Decisión:** El `task_context_t` incluye x0-x30 completo + SP + PC +
SPSR. Este es el contexto necesario para preemption desde IRQ.

**Justificación:** Preparar el terreno para el cambio de contexto
desde IRQ sin reescribir todo después.

**Estado:** Aceptado. Implementado.

### 5. Round-robin como política

**Decisión:** El scheduler es round-robin simple. Elige la
siguiente tarea en estado READY/RUNNING de forma circular.

**Justificación:** Es la política más simple. No hay prioridades ni
deadlines.

**Limitación:** No hay fairness medible. No hay prioridades.

### 6. Trampoline por tarea

**Decisión:** Toda tarea nueva arranca desde `task_trampoline`, que
recibe el puntero a la task en x19 y llama a su entry point.

**Justificación:** Permite inicializar uniformemente tareas nuevas.

### 7. Preemption desde IRQ

**Decisión:** El timer del sistema fuerza el cambio de tarea cada
100 ms. El cambio de contexto se hace desde el stub de IRQ.

**Diseño implementado:**
1. El stub de IRQ guarda x0-x30 en el stack de la tarea interrumpida.
2. Llama a `exception_handler_c(class, esr, elr, far, spsr)`.
3. El handler, para IRQ del timer:
   - `timer_irq_handler()` rearma el timer.
   - `task_tick_from_irq()` decide si cambiar de tarea.
   - Actualiza `g_preempt_from_idx` y `g_preempt_next_idx`.
4. Al retornar de C, el assembly verifica `g_preempt_next_idx`.
5. Si `>= 0`, llama a `context_switch_from_irq(from, to)`:
   - Copia x0-x30 del stack actual a `from->context`.
   - Guarda SP, PC (ELR_EL1), SPSR.
   - Copia x0-x30 de `to` al stack.
   - Cambia SP, ELR_EL1, SPSR.
   - Hace `eret`.

**Detalles críticos resueltos:**
- `SPSR` de tareas nuevas debe ser `0x305` (I=0, IRQs habilitadas),
  no `0x3C5` (I=1, IRQs enmascaradas). Si es `0x3C5`, la tarea
  arranca con IRQs deshabilitadas y el timer nunca interrumpe.
- El stack de tareas nuevas debe prepararse en `task_create` como
  si hubieran sido interrumpidas: escribir x0-x30 en el marco de
  256 bytes, con `x19 = puntero a task`.
- `task_tick_from_irq` solo debe llamarse **una vez** por tick del
  timer. Llamarla dos veces hace que el scheduler cambie a B y
  vuelva a A inmediatamente.

**Evidencia:** Verificado en QEMU virt aarch64. Las tareas A y B se
alternan correctamente con el timer a 100 ms.

## Consecuencias

- El kernel prototype tiene multitarea preemptiva funcional.
- No hay prioridades, deadlines ni SMP.
- La política es round-robin simple.
- El quantum es fijo (100 ms), no configurable.
- La UART no está sincronizada, por lo que hay basura ocasional
  en la salida (problema conocido, no del scheduler).

## Estado

**Aceptado.** Todas las decisiones (1-7) están implementadas y
verificadas.

## Ver también

- `kernel/arch/arm64/task.c` — Implementación.
- `kernel/arch/arm64/switch.S` — Cambio de contexto.
- `kernel/include/omega/task.h` — Interfaz.
- `kernel/arch/arm64/exceptions.c` — Handler de IRQ.
- `kernel/arch/arm64/exceptions_asm.S` — Stub de IRQ.
- `docs/decisions/ADR-0004-preemption-timer.md` — Decisión específica
  de preemption.
- `docs/development/README.md` — Reglas del proyecto.
