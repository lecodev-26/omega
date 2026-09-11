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
| UART funcional | ✅ Verificado |
| Vector table + excepciones | ✅ Verificado |
| ARM Generic Timer (polling) | ✅ Verificado |
| GIC + IRQ real | ✅ Verificado |
| Timer periódico con IRQ | ✅ Verificado |
| MMU | ❌ No iniciado |
| Scheduler | ❌ No iniciado |
| Kernel OMEGA | ❌ No iniciado |
| Hardware real | ❌ No disponible |

## Milestones

- **M0 — Repositorio** ✅
- **M1 — Especificación arquitectónica** (parcial: ADR-0001)
- **M2 — Prototipo host** ✅
- **M3 — Build ARM64** ✅
- **M4 — Boot en QEMU** ✅
- **M4.1 — UART + salida controlada** ✅
- **M4.2 — Vector table + excepciones** ✅
- **M4.3 — ARM Generic Timer (polling)** ✅
- **M4.4 — GIC + IRQ real** ✅
- M5 — MMU + memoria virtual
- M6 — Scheduler básico
- M7 — IPC en kernel
- M8 — Primer servicio en user space
- M9 — Boot en hardware real
- M10 — Drivers
- M11 — Sistema de archivos
- M12 — Red
- M13 — Validación de seguridad
- M14 — Entorno de desarrollo
- M15 — SO experimental usable

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
