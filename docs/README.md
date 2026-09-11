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

## Reglas

- Los documentos congelados no se modifican.
- Nuevas decisiones se registran como ADR.
- Toda afirmación factual debe tener fuente primaria.
- Separar hechos, hipótesis, decisiones y preguntas abiertas.

## Estado del kernel prototype

El kernel prototype OMEGA ha alcanzado el estado de **multitarea
cooperativa con IPC y capabilities**.

### Lo que funciona

- Boot bare-metal aarch64 en QEMU virt.
- UART (PL011).
- Vector table + excepciones (SYNC, IRQ, FIQ, SError).
- ARM Generic Timer.
- GIC (GICv2).
- IRQs reales del timer.
- Multitarea cooperativa (2+ tareas con `task_yield`).
- IPC entre tareas con colas FIFO.
- Capabilities (object capabilities) para endpoints.
- Paso de capabilities en mensajes IPC.
- Contexto extendido preparado para preemption.

### Lo que NO funciona todavía

- Preemption con timer (preparada, no activada).
- MMU (sin memoria virtual).
- User space (todo corre en EL1).
- Drivers más allá de UART, GIC y timer.
- Sistema de archivos.
- Red.

### Bloqueadores

- Ninguno impide el avance del proyecto.
- La preemption requiere más iteraciones de prueba/error.
- La MMU y el user space requieren más trabajo arquitectónico.

### Documentación relacionada

- `kernel/README.md` — Estado actual del kernel.
- `kernel/arch/arm64/README.md` — Notas de arquitectura.
- `docs/decisions/ADR-0002-kernel-prototype.md` — Decisiones de arquitectura.
- `docs/decisions/ADR-0003-scheduler.md` — Decisiones de scheduler.
- `docs/development/README.md` — Reglas del proyecto.
