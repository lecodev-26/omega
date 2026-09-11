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
| Salida controlada por UART | ✅ Verificado |
| Vector table + excepciones | ✅ Verificado |
| Handler detallado | ✅ Verificado |
| ARM Generic Timer (polling) | ✅ Verificado |
| GIC + IRQ real | ⏳ Pendiente |
| Hardware real | ❌ No disponible |
| Kernel OMEGA | ❌ No iniciado |

## Milestones

- **M0 — Repositorio** ✅
- **M1 — Especificación arquitectónica** (parcial: ADR-0001)
- **M2 — Prototipo host** ✅ (host prototype v3)
- **M3 — Build ARM64** ✅
- **M4 — Boot en QEMU** ✅
- **M4.1 — UART + salida controlada** ✅
- **M4.2 — Vector table + excepciones** ✅
- **M4.3 — ARM Generic Timer (polling)** ✅
- M5 — Boot en hardware real
- M6 — GIC + interrupciones reales
- M7 — IPC en kernel
- M8 — Primer servicio en user space
- M9 — Servicios de memoria
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
