# Architecture Decision Records (ADR)

Registro de decisiones arquitectónicas de OMEGA.

## Formato

Cada ADR sigue esta estructura:

```

ADR-XXXX: Título

Contexto

Problema

Opciones

Evidencia

Decisión

Consecuencias

Estado

```

## Estados

- **Propuesto:** En discusión.
- **Aceptado:** Decidido.
- **Parcialmente aceptado:** Algunas decisiones aceptadas, otras pendientes.
- **Rechazado:** Descartado.
- **Reemplazado:** Sustituido por otro ADR.

## Lista de ADRs

| ADR | Título | Estado |
|-----|--------|--------|
| [0001](ADR-0001-kernel-foundation.md) | Fundación del kernel de OMEGA | Propuesto |
| [0002](ADR-0002-kernel-prototype.md) | Kernel prototype — Decisiones de arquitectura | Aceptado |
| [0003](ADR-0003-scheduler.md) | Modelo de scheduler | Aceptado |
| [0004](ADR-0004-preemption-timer.md) | Preemption con timer | Aceptado |
| [0005](ADR-0005-ipc-blocking.md) | IPC blocking | Aceptado |

## ADRs pendientes

- ADR-XXXX: Modelo de MMU (memoria virtual).
- ADR-XXXX: Modelo de user space (aislamiento en EL0).
- ADR-XXXX: Modelo de drivers (kernel vs user space).
- ADR-XXXX: Sistema de archivos.
- ADR-XXXX: Red.

## Regla

Ninguna decisión arquitectónica importante se toma silenciosamente.
Toda decisión debe documentarse como ADR.
