# ADR-0002: Kernel prototype — Decisiones de arquitectura

## Contexto

En el Bloque 12 se creó `kernel/` como directorio del prototipo del kernel
OMEGA. Durante los Bloques 13–23 se añadieron multitarea cooperativa,
preemption, IPC blocking, capabilities y paso de capabilities.

Este ADR documenta las decisiones arquitectónicas tomadas en el prototipo.

## Decisiones tomadas

### 1. Target: aarch64-unknown-none (bare-metal)

**Decisión:** El kernel se compila para `aarch64-unknown-none`.

**Justificación:** Coherente con el objetivo de OMEGA de ser un sistema
operativo propio. Se ejecuta directamente sobre QEMU virt.

### 2. Toolchain: clang + ld.lld + llvm-objcopy

**Decisión:** Usar LLVM.

**Justificación:** Clang en Termux soporta el target `aarch64-unknown-none`.
`ld.lld` se invoca directamente porque clang tiene problemas para invocar
el linker en targets bare-metal.

**Ver:** `docs/development/README.md` — Regla sobre linker bare-metal.

### 3. Compilación sin FP/SIMD

**Decisión:** Compilar con `-mgeneral-regs-only`.

**Justificación:** En bare-metal aarch64, FP/SIMD está deshabilitado por
defecto. El compilador puede generar instrucciones SIMD que causan
excepciones irrecuperables.

### 4. Alineación de structs a 8/16 bytes

**Decisión:** Toda struct accesible con instrucciones de 8 bytes debe
estar alineada (`__attribute__((aligned(8)))` o `aligned(16)`).

**Justificación:** Sin MMU, los accesos no alineados generan Alignment fault.

### 5. Multitarea preemptiva (supera la cooperativa)

**Decisión:** El scheduler es preemptivo con timer a 100 ms. Las tareas
también pueden ceder el control con `task_yield()`.

**Justificación:** La preemption es la base de un scheduler real. El
cambio de contexto desde IRQ requiere contexto extendido.

**Ver:** ADR-0004 — Preemption con timer.

### 6. Contexto de tarea extendido

**Decisión:** `task_context_t` guarda x0-x30 + SP + PC + SPSR.

**Justificación:** Es el contexto necesario para reanudar una tarea
interrumpida por IRQ. El cambio cooperativo usa solo los callee-saved,
pero el contexto extendido permite preemption.

**Ver:** `docs/development/README.md` — Regla sobre contexto extendido.

### 7. Trampoline por tarea

**Decisión:** Toda tarea nueva arranca desde `task_trampoline`, que recibe
el puntero a la task en x19 y llama a su entry point.

**Justificación:** Permite inicializar uniformemente tareas nuevas.

### 8. IPC blocking con capabilities

**Decisión:** El IPC usa endpoints identificados por índice, con colas
FIFO de 16 mensajes de 64 bytes de payload. `send`/`recv` bloquean si
el buzón está lleno/vacío. El acceso a endpoints está mediado por
capabilities.

**Justificación:** IPC real, no polling. Control de acceso real.

**Ver:** ADR-0005 — IPC blocking.

### 9. Capabilities y paso de capabilities

**Decisión:** Cada tarea tiene una tabla de 16 capabilities. Las
capabilities designan endpoints con derechos (`READ`, `WRITE`, `GRANT`,
`REVOKE`). El paso de capabilities en mensajes sigue el principio de
no incremento de derechos.

**Ver:** `docs/development/README.md` — Regla sobre capabilities.

### 10. Sincronización con `daif` save/restore

**Decisión:** En lugar de spinlocks, el kernel deshabilita IRQs en
secciones críticas (UART, IPC, `task_yield`). En un sistema uniprocesador,
es suficiente.

**Justificación:** Evita deadlocks por reentrada.

**Ver:** ADR-0005 — IPC blocking.

## Consecuencias

- El kernel prototype es funcional en QEMU virt aarch64.
- Cada decisión tiene una limitación conocida.
- La arquitectura conceptual (0.1.x) sigue siendo la referencia a largo
  plazo, pero el prototype es un subconjunto simplificado.

## Estado

**Aceptado.** Parcialmente superado por ADR-0003, ADR-0004, ADR-0005.

## Pendientes para próximos ADRs

- ADR-XXXX: Modelo de MMU (cuando se implemente).
- ADR-XXXX: Modelo de user space (cuando se implemente).
- ADR-XXXX: Modelo de drivers (cuando se implemente).
