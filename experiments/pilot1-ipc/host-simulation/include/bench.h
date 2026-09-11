/*
 * OMEGA — Pilot 1 IPC — Host simulation
 * bench.h
 */

#ifndef OMEGA_PILOT1_BENCH_H
#define OMEGA_PILOT1_BENCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PILOT1_SIZE_8     8
#define PILOT1_SIZE_64    64
#define PILOT1_SIZE_512   512
#define PILOT1_SIZE_4096  4096

#define PILOT1_ITERATIONS 10000
#define PILOT1_WARMUP     1000
#define PILOT1_NUM_SIZES  4

typedef struct {
    uint32_t size;
    uint32_t iteration;
    uint64_t latency_ns;
} pilot1_sample_t;

int pilot1_run_size(uint32_t payload_size,
                    uint32_t iterations,
                    uint32_t warmup,
                    FILE *out);

int pilot1_run_all(const char *output_path);

#endif /* OMEGA_PILOT1_BENCH_H */
