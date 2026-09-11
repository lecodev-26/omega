# Changelog

## [Unreleased]

### Added
- Estructura inicial del repositorio.
- Documentación fundacional (README, ROADMAP, CONTRIBUTING, SECURITY).
- Directorios preparados para docs, kernel, user, tools, tests, experiments.

## [0.1.0] — 2026-09-11

### Added
- Estructura inicial del repositorio OMEGA.
- Documentación fundacional (README, ROADMAP, CONTRIBUTING, SECURITY).
- Documentación estructurada en `docs/` con índices y ADR-0001.
- Host prototype v3: mensajes, endpoints, capabilities (con delegación), servicios (echo, time).
- Makefile con target `arm64`.
- GitHub Actions CI: docs-check + host-build.
- Simulación host del Piloto 1 IPC: benchmark de 10.000 iteraciones × 4 tamaños.
- Script de análisis en Python con mediana, p95, p99 y CI bootstrap.

### Notes
- No hay kernel funcional.
- No hay sistema operativo ejecutable.
- La simulación del Piloto 1 NO es el experimento científico real.
