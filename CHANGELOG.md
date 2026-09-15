# Changelog

## [Unreleased]

### Added
- Repositorio inicial con documentación estructurada.
- Host prototype v3: mensajes, endpoints, capabilities, servicios.
- Simulación host del Piloto 1 IPC con análisis estadístico.
- Kernel prototype:
  - Boot bare-metal aarch64 en QEMU virt.
  - UART PL011.
  - Vector table + excepciones (SYNC, IRQ, FIQ, SError).
  - ARM Generic Timer.
  - GIC + IRQs reales.
  - Multitarea cooperativa.
  - IPC básico (endpoints + mensajes).
  - Capabilities (object capabilities para endpoints IPC).
  - Paso de capabilities en mensajes IPC.
  - Preemption con timer (cambio de contexto desde IRQ).
- Documentación:
  - 19 documentos conceptuales (0.1.0–0.1.18).
  - Requisitos y propiedades verificables (R1-FINAL).
  - Diseño experimental (R2).
  - Protocolo Piloto 1.
  - ADR-0001: Fundación del kernel.
  - ADR-0002: Kernel prototype.
  - ADR-0003: Modelo de scheduler.
  - ADR-0004: Preemption con timer.
  - Documento de brecha conceptual.

### Fixed
- `SPSR` de tareas nuevas: `0x305` (I=0) en lugar de `0x3C5` (I=1).
  Sin esto, las tareas arrancaban con IRQs enmascaradas y el timer
  nunca interrumpía.
- Stack de tareas nuevas: preparar marco de 256 bytes con x0-x30
  en `task_create`. Sin esto, `context_switch_from_irq` cargaba
  basura en `x19` al cambiar a una tarea nueva.
- Doble llamada a `task_tick_from_irq` en `exception_handler_c`.
  La segunda llamada revertía el cambio de tarea.

### Notes
- No hay hardware real disponible todavía.
- El kernel prototype corre únicamente en QEMU.
- No hay MMU ni user space todavía.
- La UART no está sincronizada: puede haber basura ocasional en
  la salida cuando el timer interrumpe mientras se imprime.
