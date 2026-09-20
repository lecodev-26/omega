# OMEGA — Documentación

Índice central de la documentación del proyecto.

## Estructura

| Directorio | Contenido |
|------------|-----------|
| [vision/](vision/) | Visión, principios y contexto del proyecto |
| [architecture/](architecture/) | Especificación arquitectónica |
| [requirements/](requirements/) | Requisitos y propiedades verificables |
| [design/](design/) | Diseño detallado de componentes |
| [experiments/](experiments/) | Protocolos experimentales y resultados |
| [decisions/](decisions/) | Architecture Decision Records (ADR) |
| [development/](development/) | Guía de desarrollo |
| [hardware/](hardware/) | Documentación de plataformas objetivo |

## Estado de la documentación

- **Investigación conceptual (0.1.0–0.1.18):** Congelada.
- **Requisitos (R1-FINAL):** Congelado.
- **Diseño experimental (R2):** Congelado.
- **Protocolo Piloto 1:** Cerrado como propuesta.

## Estado del kernel prototype

El kernel prototype OMEGA ha alcanzado el estado de **multitarea
preemptiva con IPC blocking y capabilities**.

### Lo que funciona

- Boot bare-metal aarch64 en QEMU virt.
- UART (PL011).
- Vector table + excepciones (SYNC, IRQ, FIQ, SError).
- ARM Generic Timer.
- GIC (GICv2).
- IRQs reales del timer.
- Multitarea preemptiva (timer a 100 ms).
- Multitarea cooperativa (`task_yield`).
- IPC blocking (send/recv esperan si el buzón está lleno/vacío).
- Capabilities (object capabilities) para endpoints.
- Paso de capabilities en mensajes IPC.
- Contexto extendido (x0-x30 + SP + PC + SPSR).

### Lo que NO funciona todavía

- MMU (sin memoria virtual).
- User space (todo corre en EL1).
- Drivers más allá de UART, GIC y timer.
- Sistema de archivos.
- Red.
- SMP (un solo CPU).

### Documentación relacionada

- `kernel/README.md` — Estado actual del kernel.
- `kernel/arch/arm64/README.md` — Notas de arquitectura.
- `docs/decisions/ADR-0002-kernel-prototype.md` — Decisiones de arquitectura.
- `docs/decisions/ADR-0003-scheduler.md` — Modelo de scheduler.
- `docs/decisions/ADR-0004-preemption-timer.md` — Preemption con timer.
- `docs/decisions/ADR-0005-ipc-blocking.md` — IPC blocking.
- `docs/development/README.md` — Reglas del proyecto.
