# Arquitectura

Esta sección contiene la especificación arquitectónica de OMEGA.

## Estado

**La arquitectura NO está completamente especificada.**

La dirección arquitectónica actual es:

> Microkernel + capabilities + aislamiento fuerte + servicios en user space
> + IPC explícito + composición modular + verificabilidad + portabilidad.

Pero esta arquitectura **no se considera demostrada ni definitiva**.

## Secciones planificadas

- [ ] `capabilities.md` — Modelo de capabilities
- [ ] `ipc.md` — Modelo de IPC
- [ ] `memory.md` — Modelo de memoria
- [ ] `threads.md` — Modelo de threads y scheduling
- [ ] `services.md` — Modelo de servicios
- [ ] `boot.md` — Modelo de arranque
- [ ] `security.md` — Modelo de seguridad

## Reglas

- No inventar arquitectura sin evidencia.
- Las decisiones importantes se registran como ADR en `../decisions/`.
- Toda especificación debe distinguir entre:
  - [HECHO] lo documentado.
  - [HIPÓTESIS] lo propuesto.
  - [PREGUNTA ABIERTA] lo no resuelto.
