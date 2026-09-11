/*
 * OMEGA — Pilot 1 IPC — Host simulation
 * main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bench.h"

int main(int argc, char **argv) {
    const char *output = "results/pilot1_host.csv";
    if (argc > 1) output = argv[1];

    printf("=== OMEGA Pilot 1 — Host Simulation ===\n");
    printf("ESTO NO ES EL EXPERIMENTO CIENTÍFICO REAL.\n");
    printf("Es una simulación del protocolo sobre el host prototype.\n\n");
    printf("Output: %s\n", output);
    printf("Iteraciones: %u (+ %u warm-up)\n",
           PILOT1_ITERATIONS, PILOT1_WARMUP);
    printf("Tamaños: 8, 64, 512, 4096 bytes\n\n");

    if (pilot1_run_all(output) != 0) {
        fprintf(stderr, "ERROR: benchmark falló\n");
        return 1;
    }

    printf("\n=== Benchmark completado ===\n");
    printf("Resultados en: %s\n", output);
    return 0;
}
