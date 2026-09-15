# ADR-0004: Preemption con timer

## Contexto

El kernel prototype tenía multitarea cooperativa (`task_yield()`).
La preemption desde IRQ estaba preparada arquitectónicamente
(contexto extendido, `context_switch_from_irq`, `task_tick_from_irq`)
pero no conectada al stub de IRQ.

Este ADR documenta la decisión de activar la preemption y los
detalles técnicos resueltos.

## Problema

¿Cómo forzar el cambio de tarea desde el handler de IRQ del timer
sin corromper el contexto de la tarea interrumpida?

## Opciones

### Opción A — No implementar preemption

Mantener solo multitarea cooperativa.

**Ventajas:**
- Simplicidad.
- Menos código en el stub de IRQ.

**Desventajas:**
- Una tarea que no cede monopoliza la CPU.
- No es un scheduler real.
- Bloquea el avance hacia un SO usable.

### Opción B — Preemption con timer (elegida)

El timer del sistema interrumpe cada 100 ms. El stub de IRQ decide
si cambiar de tarea y, si es así, hace el cambio de contexto antes
de retornar.

**Ventajas:**
- Multitarea real.
- Base para prioridades y deadlines.
- Modelo estándar en SO modernos.

**Desventajas:**
- Complejidad del stub de IRQ.
- Riesgo de corromper el contexto si no se guarda todo.
- Necesita contexto extendido (x0-x30 + SP + PC + SPSR).

### Opción C — Preemption con hardware dedicado

Usar un temporizador dedicado o un mecanismo de hardware específico.

**Ventajas:**
- Menos overhead.

**Desventajas:**
- No portable.
- Más complejo.
- No disponible en QEMU virt de forma genérica.

## Evidencia

- ARM Generic Timer (`CNTP_TVAL_EL0`, `CNTP_CTL_EL0`) en QEMU virt
  dispara PPI 30.
- GICv2 enruta la IRQ al CPU.
- El stub de IRQ ya guarda x0-x30 en el stack de la tarea.
- `context_switch_from_irq` ya estaba preparado pero desconectado.

## Decisión

**Activar preemption con timer.**

Implementación:

1. `timer_irq_handler()` rearma el timer a 10 ms (en la práctica,
   el quantum visible es 100 ms por el bucle de las tareas).
2. `task_tick_from_irq()` decide la siguiente tarea y actualiza
   `g_preempt_from_idx` y `g_preempt_next_idx`.
3. El stub de IRQ, al retornar de C, verifica `g_preempt_next_idx`:
   - Si `< 0`: retorno normal (restaurar x0-x30 del stack + `eret`).
   - Si `>= 0`: llamar a `context_switch_from_irq(from, to)`.
4. `context_switch_from_irq` hace el cambio completo y `eret`.

## Detalles críticos resueltos

### 1. SPSR de tareas nuevas

**Problema:** Las tareas nuevas arrancaban con `SPSR = 0x3C5`
(DAIF todo enmascarado). Eso significa `I=1`, IRQs deshabilitadas.
El timer nunca interrumpía.

**Solución:** `SPSR = 0x305` (D=1, A=1, I=0, F=0). IRQs habilitadas.

### 2. Stack de tareas nuevas

**Problema:** `context_switch_from_irq` carga x0-x30 desde el stack
de la tarea destino. Si el stack no está preparado, carga basura,
especialmente `x19` (que debería ser el puntero a la task).

**Solución:** En `task_create`, escribir x0-x30 en el marco de 256
bytes del stack de la tarea nueva, con `x19 = puntero a task`.

### 3. Doble llamada a `task_tick_from_irq`

**Problema:** `exception_handler_c` llamaba a `task_tick_from_irq_diag2`
dos veces. La primera decidía cambiar a B, la segunda decidía volver
a A. Resultado: B nunca arrancaba.

**Solución:** Llamar a `task_tick_from_irq` una sola vez por tick.

### 4. `eret` y SP

**Detalle:** `eret` no restaura SP. Usa `SP_EL1` directamente. Por
eso `context_switch_from_irq` ajusta `sp` al SP original de la tarea
antes de `eret`.

### 5. `context_switch_from_irq` y `x0, x1`

**Detalle:** Al cargar x0-x30 del stack a los registros, x0 y x1
se cargan primero, y x2, x3 se cargan al final (porque x2, x3 se
usan como temporales durante el proceso). Si no se hace así, x2 y x3
se corrompen.

## Consecuencias

- El kernel prototype tiene multitarea preemptiva funcional.
- Verificado en QEMU virt aarch64: tareas A y B se alternan.
- Quantum fijo (100 ms), no configurable.
- No hay prioridades.
- No hay SMP.
- La UART no está sincronizada (basura ocasional en la salida).

## Estado

**Aceptado.** Implementado y verificado.

## Ver también

- `docs/decisions/ADR-0003-scheduler.md` — Modelo de scheduler.
- `kernel/arch/arm64/task.c` — `task_tick_from_irq`, `task_create`.
- `kernel/arch/arm64/switch.S` — `context_switch_from_irq`.
- `kernel/arch/arm64/exceptions_asm.S` — Stub de IRQ.
- `kernel/arch/arm64/exceptions.c` — `exception_handler_c`.
