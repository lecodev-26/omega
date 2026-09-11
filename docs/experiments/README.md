# Experimentos

Esta sección contiene los protocolos experimentales y sus resultados.

## Piloto 1 — IPC

**Estado:** Protocolo cerrado como propuesta. Ejecución pendiente de hardware.

**Plataforma:** IMX8MP-EVK (candidata documental)

**Objetivo:** Medir latencia de transacciones IPC request-response en seL4, QNX y Linux.

**Workload:**
- Request-response ping-pong
- Tamaños: 8, 64, 512, 4096 bytes
- Modo: intra-core (cliente y servidor en el mismo Cortex-A53)

**Métricas:**
- Latencia de round-trip
- Mediana, p95, p99
- IC bootstrap

## Estructura

- `pilot1-ipc/raw/` — Datos brutos
- `pilot1-ipc/processed/` — Datos procesados
- `pilot1-ipc/plots/` — Gráficos
- `pilot1-ipc/reports/` — Informes
- `pilot1-ipc/metadata/` — Metadatos

## Reglas

- No modificar datos brutos.
- Documentar todas las condiciones de ejecución.
- Separar resultados de interpretaciones.
