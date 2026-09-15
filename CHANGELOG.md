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
  - IPC blocking (send/recv esperan si el buzón está lleno/vacío).
- Documentación:
  - 19 documentos conceptuales (0.1.0–0.1.18).
  - Requisitos y propiedades verificables (R1-FINAL).
  - Diseño experimental (R2).
  - Protocolo Piloto 1.
  - ADR-0001: Fundación del kernel.
  - ADR-0002: Kernel prototype.
  - ADR-0003: Modelo de scheduler.
  - ADR-0004: Preemption con timer.
  - ADR-0005: IPC blocking.
  - Documento de brecha conceptual.

### Fixed
- `SPSR` de tareas nuevas: `0x305` (I=0) en lugar de `0x3C5` (I=1).
- Stack de tareas nuevas: preparar marco de 256 bytes con x0-x30
  en `task_create`.
- Doble llamada a `task_tick_from_irq` en `exception_handler_c`.
- `g_current` no se actualizaba en `task_yield` (arranque inicial y
  cambio cooperativo).
- `task_yield` no era atómico: el timer podía interrumpir en medio
  del cambio de contexto.
- `ipc_send`/`ipc_recv` no eran atómicos: el timer podía interrumpir
  entre `ep->count++` y `task_unblock()`.

### Notes
- No hay hardware real disponible todavía.
- El kernel prototype corre únicamente en QEMU.
- No hay MMU ni user space todavía.
- La UART usa daif save/restore (IRQs off) en vez de spinlock.
