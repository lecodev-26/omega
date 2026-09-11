# OMEGA — Roadmap

## Estado actual

| Fase | Estado |
|------|--------|
| Investigación conceptual (0.1.0–0.1.18) | ✅ Congelado |
| Requisitos y propiedades verificables | ✅ Congelado |
| Diseño experimental (R2) | ✅ Congelado |
| Piloto 1 — protocolo | ✅ Cerrado como propuesta |
| Repositorio | ✅ Creado |
| Documentación estructurada | ✅ Creada |
| Host prototype v3 | ✅ Funcional |
| Simulación host Piloto 1 | ✅ Funcional |
| ARM64 build (nativo) | ✅ Verificado |
| Bare-metal boot en QEMU | ✅ Verificado |
| Toolchain bare-metal | ✅ Verificado |
| Hardware real | ❌ No disponible |
| Kernel OMEGA | ❌ No iniciado |

## Milestones

- **M0 — Repositorio** ✅
- **M1 — Especificación arquitectónica** (parcial: ADR-0001)
- **M2 — Prototipo host** ✅ (host prototype v3)
- **M3 — Build ARM64** ✅
- **M4 — Boot en QEMU** ✅ (bare-metal minimal)
- M5 — Boot en hardware real
- M6 — IPC en kernel
- M7 — Primer servicio en user space
- M8 — Servicios de memoria
- M9 — Drivers
- M10 — Sistema de archivos
- M11 — Red
- M12 — Validación de seguridad
- M13 — Entorno de desarrollo
- M14 — SO experimental usable

## Lo que NO se hace ahora

- No comprar hardware por impulso.
- No crear una distribución.
- No escribir 100.000 líneas de kernel.
- No implementar GUI, navegador, package manager.
- No hacer benchmarks sin hardware.
- No afirmar que algo funciona sin probarlo.
- No convertir simulaciones en evidencia experimental.
- No reabrir documentos cerrados sin contradicción real.

## Principio rector

> Conocer → comparar → experimentar → decidir → construir.
