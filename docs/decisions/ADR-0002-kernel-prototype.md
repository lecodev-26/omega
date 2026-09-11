# ADR-0002: Kernel prototype — Decisiones de arquitectura

## Contexto

En el Bloque 12 se creó `kernel/` como directorio del prototipo del kernel
OMEGA. Durante los Bloques 13 y 14 se añadieron multitarea cooperativa y
IPC básico. Este ADR documenta las decisiones arquitectónicas tomadas en
el prototipo y sus justificaciones.

## Decisiones tomadas

### 1. Target: aarch64-unknown-none (bare-metal)

**Decisión:** El kernel se compila para `aarch64-unknown-none`, sin
dependencia de ningún sistema operativo subyacente.

**Justificación:** Coherente con el objetivo de OMEGA de ser un sistema
operativo propio. Se ejecuta directamente sobre QEMU virt.

### 2. Toolchain: clang + ld.lld + llvm-objcopy

**Decisión:** Usar LLVM (clang como compilador, ld.lld como linker,
llvm-objcopy para generar el binario raw).

**Justificación:** Clang en Termux soporta el target `aarch64-unknown-none`.
`ld.lld` se invoca directamente (no mediante clang) porque clang en Termux
tiene problemas invocando el linker para targets bare-metal.

**Ver:** `docs/development/README.md` — Regla sobre linker bare-metal.

### 3. Compilación sin FP/SIMD

**Decisión:** Compilar con `-mgeneral-regs-only`.

**Justificación:** En bare-metal aarch64, el acceso a registros FP/SIMD
está deshabilitado por defecto (via `CPACR_EL1.FPEN`). El compilador no
lo sabe y puede generar instrucciones SIMD que causan excepciones
irrecuperables.

**Ver:** `docs/development/README.md` — Regla sobre FP/SIMD.

### 4. Alineación de structs a 8/16 bytes

**Decisión:** Toda struct accesible por el compilador con instrucciones
de 8 bytes debe estar alineada adecuadamente (`__attribute__((aligned(8)))`
o `aligned(16)`).

**Justificación:** Sin MMU, en bare-metal aarch64, los accesos no alineados
a la instrucción generan Alignment fault. El compilador puede generar
`stur x8` o `stp` que requieren alineación.

**Ver:** `docs/development/README.md` — Regla sobre alineación.

### 5. Multitarea cooperativa (no preemptiva todavía)

**Decisión:** Las tareas ceden el control explícitamente con `task_yield()`.

**Justificación:** El cambio de contexto cooperativo es más simple y no
requiere modificar el handler de IRQ. La preemption con timer se deja
para un bloque posterior.

**Limitación:** Las tareas deben ser cooperativas. Una tarea que no ceda
el control monopoliza la CPU.

### 6. Contexto de tarea: callee-saved solo

**Decisión:** El `task_context_t` guarda solo los registros callee-saved
(x19-x28, x29/FP, x30/LR, SP, PC).

**Justificación:** Suficiente para el cambio de contexto cooperativo
(llamada a función normal). La preemption requerirá guardar x0-x30 completo.

### 7. Trampoline por tarea

**Decisión:** Toda tarea nueva arranca desde `task_trampoline`, que recibe
el puntero a la task en x19 y llama a su entry point.

**Justificación:** Permite inicializar uniformemente tareas nuevas. Si
la tarea retorna, `task_finished()` la marca como terminada.

### 8. IPC básico (endpoints + mensajes)

**Decisión:** El IPC usa endpoints identificados por índice (0..MAX_TASKS-1)
y mensajes con payload de 64 bytes.

**Justificación:** Modelo simple que valida el mecanismo. No hay
capabilities todavía (acceso por número de endpoint). No hay blocking.

**Limitación:** No hay control de acceso. Cualquier tarea puede enviar
a cualquier endpoint.

## Consecuencias

- El kernel prototype es funcional pero limitado.
- Cada decisión tiene una limitación conocida y una mejora futura.
- La arquitectura conceptual (0.1.x) sigue siendo la referencia a largo
  plazo, pero el prototype es un subconjunto simplificado.

## Estado

**Aceptado.**

## Pendientes para próximos ADRs

- ADR-XXXX: Modelo de preemption (cuando se implemente).
- ADR-XXXX: Modelo de capabilities (cuando se implemente).
- ADR-XXXX: Modelo de MMU (cuando se implemente).
- ADR-XXXX: Modelo de user space (cuando se implemente).
