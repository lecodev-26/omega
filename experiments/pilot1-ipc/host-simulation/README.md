# Pilot 1 IPC — Host Simulation

**ESTADO: SIMULACIÓN.**

**ESTO NO ES EL EXPERIMENTO CIENTÍFICO REAL.**

Es una simulación del protocolo del Piloto 1 sobre el host prototype.
Su objetivo es preparar toda la infraestructura experimental antes de
tener acceso al hardware.

## Qué simula

- Transacciones request-response entre dos endpoints en memoria.
- Los 4 tamaños del Piloto 1: 8, 64, 512, 4096 bytes.
- Instrumentación con `clock_gettime(CLOCK_MONOTONIC)`.
- 10.000 iteraciones por tamaño + 1.000 de warm-up.
- Output en CSV.

## Qué NO simula

- No hay kernel.
- No hay scheduling real.
- No hay IPC real entre procesos.
- No hay aislamiento real.
- Los resultados **no reflejan** el rendimiento de un IPC real.

## Ejecutar

```bash
cd experiments/pilot1-ipc/host-simulation
make CC=clang
make run
```

Esto genera results/pilot1_host.csv.

Analizar

```bash
python3 scripts/analyze.py results/pilot1_host.csv
```

Muestra mediana, p95, p99, stdev y CI bootstrap 95%.

Por qué existe

El protocolo del Piloto 1 está cerrado en cuanto a metodología.
Cuando llegue el hardware (IMX8MP-EVK), solo habrá que cambiar la
plataforma de ejecución y volver a correr el benchmark.

Esta simulación sirve para:

1. Validar que el protocolo es ejecutable.
2. Verificar que el pipeline de datos funciona.
3. Detectar problemas de instrumentación antes de tener hardware.
4. Tener un baseline conceptual para comparar después.

Advertencia metodológica

Los datos de esta simulación no son evidencia sobre el rendimiento
de OMEGA, seL4, QNX o Linux. Son datos del host prototype ejecutándose
en Termux, midiendo operaciones en memoria.

Cuando se ejecute el Piloto 1 real, los resultados sustituirán a estos.
