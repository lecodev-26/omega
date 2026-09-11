# ADR-0001: Fundación del kernel de OMEGA

## Contexto

OMEGA es un proyecto de investigación a largo plazo para construir un
sistema operativo de nueva generación. La investigación conceptual
(0.1.0–0.1.18) apunta hacia una arquitectura basada en microkernel +
capabilities.

Sin embargo, la implementación concreta todavía no está decidida.

## Problema

¿Sobre qué base técnica debe construirse OMEGA?

## Opciones

### Opción A — OMEGA sobre/adaptado a seL4

- **Ventaja:** seL4 está verificado formalmente. Proporciona IPC, capabilities,
  scheduling.
- **Desventaja:** seL4 es un microkernel ya existente; OMEGA sería una capa
  encima. La arquitectura propia quedaría limitada.

### Opción B — Microkernel propio desde cero

- **Ventaja:** Control total sobre la arquitectura.
- **Desventaja:** Muy costoso. Requiere años de trabajo. Riesgo de no llegar
  a un sistema funcional.

### Opción C — Otra base experimental

- **Ventaja:** Flexibilidad.
- **Desventaja:** Requiere evaluación específica.

### Opción D — Prototipo host/simulado antes del kernel real

- **Ventaja:** Permite explorar conceptos sin hardware.
- **Desventaja:** No demuestra funcionamiento real.

## Evidencia

- La investigación previa (0.1.2) apunta hacia microkernel + capabilities.
- No hay evidencia experimental todavía.
- No hay hardware disponible.

## Decisión

**Pendiente.** No se decide ahora.

Se construirá primero un **prototipo host** que permita explorar conceptos
(IPC, capabilities, servicios) sin depender de hardware ni de kernel real.

La decisión sobre la base del kernel real se tomará cuando:
1. Exista el prototipo host.
2. Exista hardware disponible.
3. Se haya experimentado con el Piloto 1.

## Consecuencias

- No se bloquea el proyecto por falta de decisión.
- Se puede avanzar en prototipos, tests, toolchain.
- La decisión arquitectónica real se toma con más información.

## Estado

**Propuesto.**

## Referencias

- `docs/vision/` — Fases 0.1.0–0.1.18
- `docs/architecture/` — Especificación pendiente
- `ROADMAP.md` — Milestones
