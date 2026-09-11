# ADR-0003: Modelo de scheduler

## Contexto

El kernel prototype incluye un scheduler de tareas. Actualmente es
cooperativo: las tareas ceden el control con `task_yield()`. La
multitarea preemptiva (con el timer forzando el cambio) está preparada
pero no activada.

Este ADR documenta las decisiones tomadas y las pendientes.

## Decisiones tomadas

### 1. Tareas como unidades de ejecución

**Decisión:** Cada tarea es una función C con su propio stack (4 KB)
y contexto. Máximo 8 tareas concurrentes.

**Justificación:** Modelo simple que valida el mecanismo. Ampliable
en el futuro.

### 2. Multitarea cooperativa (fase actual)

**Decisión:** Las tareas ceden el control explícitamente con
`task_yield()`.

**Justificación:** El cambio de contexto cooperativo es más simple
y no requiere modificar el handler de IRQ. La preemption desde IRQ
es más compleja (guarda contexto completo, modifica ELR_EL1 y SPSR_EL1).

**Limitación:** Una tarea que no cede el control monopoliza la CPU.

### 3. Contexto cooperativo: callee-saved

**Decisión:** El `context_switch` cooperativo guarda/restaura solo los
registros callee-saved (x19-x30) + SP + PC.

**Justificación:** El cambio de contexto se hace desde una llamada
a función normal. Los caller-saved (x0-x18) ya están guardados en el
stack del llamador.

### 4. Contexto extendido (preparado para preemption)

**Decisión:** El `task_context_t` incluye x0-x30 completo + SP + PC +
SPSR. Este es el contexto necesario para preemption desde IRQ.

**Justificación:** Preparar el terreno para el cambio de contexto
desde IRQ sin reescribir todo después.

**Estado:** El contexto extendido está preparado. El cambio de
contexto desde IRQ (`context_switch_from_irq` en assembly) también
está preparado, pero **no está conectado al stub de IRQ todavía**.

### 5. Round-robin como política

**Decisión:** El scheduler actual es round-robin simple. Elige la
siguiente tarea en estado READY/RUNNING de forma circular.

**Justificación:** Es la política más simple. No hay prioridades ni
deadlines.

**Limitación:** No hay fairness medible. No hay prioridades.

### 6. Trampoline por tarea

**Decisión:** Toda tarea nueva arranca desde `task_trampoline`, que
recibe el puntero a la task en x19 y llama a su entry point.

**Justificación:** Permite inicializar uniformemente tareas nuevas.

## Decisiones pendientes (18b/18c)

### 7. Preemption desde IRQ

**Estado:** Preparado pero no activado.

**Diseño propuesto:**
1. El stub de IRQ guarda x0-x30 en el stack de la tarea.
2. Llama a `exception_handler_c`.
3. El handler llama a `task_tick_from_irq()` que decide si cambiar
   de tarea. Actualiza `g_preempt_next_idx`.
4. Al retornar de C, el assembly verifica `g_preempt_next_idx`.
5. Si hay cambio, llama a `context_switch_from_irq(from, to)` que:
   - Copia x0-x30 del stack actual a `from->context`.
   - Guarda SP, PC (ELR_EL1), SPSR.
   - Copia x0-x30 de `to` al stack.
   - Cambia SP, ELR_EL1, SPSR.
   - Hace `eret`.

**Dificultad:** El diseño está claro, pero la integración con el
stub de excepción en assembly es delicada. Requiere muchas iteraciones
de prueba/error.

**Bloqueador:** No bloquea el avance del proyecto. La multitarea
cooperativa funciona.

## Consecuencias

- El kernel prototype funciona con multitarea cooperativa.
- La preemption está preparada arquitectónicamente pero no activada.
- No hay prioridades, deadlines ni SMP.
- La política es round-robin simple.

## Estado

**Aceptado parcialmente.** Las decisiones 1-6 están aceptadas. La
decisión 7 (preemption) está pendiente.

## Ver también

- `kernel/arch/arm64/task.c` — Implementación.
- `kernel/arch/arm64/switch.S` — Cambio de contexto.
- `kernel/include/omega/task.h` — Interfaz.
- `docs/development/README.md` — Reglas del proyecto.
