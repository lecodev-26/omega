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
| Kernel prototype — boot | ✅ Verificado |
| Kernel prototype — UART | ✅ Verificado |
| Kernel prototype — excepciones | ✅ Verificado |
| Kernel prototype — timer | ✅ Verificado |
| Kernel prototype — IRQs reales | ✅ Verificado |
| Kernel prototype — multitarea cooperativa | ✅ Verificado |
| Kernel prototype — IPC básico | ✅ Verificado |
| Kernel prototype — capabilities | ✅ Verificado |
|  Kernel prototype — paso de cap en mensajes | ✅ Verificado |
| Kernel prototype — preemption con timer | ⏳ Pendiente |
| Kernel prototype — MMU | ❌ No iniciado |
| Kernel prototype — user space | ❌ No iniciado |
| Hardware real | ❌ No disponible |

## Milestones

- **M0 — Repositorio** ✅
- **M1 — Especificación arquitectónica** (parcial: ADR-0001, ADR-0002)
- **M2 — Prototipo host** ✅
- **M3 — Build ARM64** ✅
- **M4 — Boot en QEMU** ✅
- **M4.1 — UART + salida controlada** ✅
- **M4.2 — Vector table + excepciones** ✅
- **M4.3 — ARM Generic Timer (polling)** ✅
- **M4.4 — GIC + IRQ real** ✅
- **M5 — Multitarea cooperativa** ✅
- **M5.1 — IPC básico** ✅
- **M5.2 — Capabilities** ✅
- **M5.3 — Paso de cap en mensajes IPC** ✅
- M6 — Preemption con timer (18a completado, 18b pendiente)
- M7 — MMU + memoria virtual
- M8 — User space
- M9 — Drivers
- M10 — Boot en hardware real
- M11 — Sistema de archivos
- M12 — Red
- M13 — Validación de seguridad
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
