# OMEGA

[![CI](https://github.com/lecodev-26/omega/actions/workflows/ci.yml/badge.svg)](https://github.com/lecodev-26/omega/actions/workflows/ci.yml)

OMEGA es un proyecto open source de investigación y desarrollo a muy largo plazo
para construir un sistema operativo de nueva generación.

## Estado

**Fase actual:** Prototipo funcional en QEMU. Consolidación documental.

**Existe:**

- Kernel prototype funcional en QEMU virt aarch64.
- Boot bare-metal, UART, excepciones, timer, GIC.
- Multitarea preemptiva con timer.
- IPC blocking con capabilities.
- Paso de capabilities en mensajes IPC.
- Host prototype v3 (banco de pruebas en user space).
- Simulación host del Piloto 1 IPC.

**No existe todavía:**

- Kernel en hardware real.
- MMU / memoria virtual.
- User space (EL0).
- Drivers más allá de UART, GIC, timer.
- Sistema de archivos.
- Red.
- Resultados experimentales en hardware.

**El kernel prototype corre únicamente en QEMU virt aarch64.**
**No se afirma que funcione en hardware real.**

## Visión

OMEGA investiga una arquitectura propia basada en:

- Microkernel
- Capabilities
- Aislamiento fuerte
- Servicios en user space
- IPC explícito
- Composición modular
- Verificabilidad
- Portabilidad

Esta arquitectura **no se considera demostrada ni definitiva**. Las decisiones
futuras pueden cambiar si la evidencia experimental o técnica lo exige.

## Principio rector

> Conocer → comparar → experimentar → decidir → construir.

## Documentación

- [Visión](docs/vision/)
- [Arquitectura](docs/architecture/)
- [Requisitos](docs/requirements/)
- [Experimentos](docs/experiments/)
- [Decisiones (ADR)](docs/decisions/)
- [Desarrollo](docs/development/)
- [Hardware](docs/hardware/)

## Roadmap

Ver [ROADMAP.md](ROADMAP.md).

## Licencia

Pendiente de decisión. Ver [LICENSE](LICENSE).

## Contribuir

Ver [CONTRIBUTING.md](CONTRIBUTING.md).

## Seguridad

Ver [SECURITY.md](SECURITY.md).
