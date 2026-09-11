/*
 * OMEGA — Pilot 1 IPC — Host simulation
 * bench.c
 *
 * SIMULACIÓN del protocolo del Piloto 1.
 *
 * Ejecuta transacciones request-response entre dos endpoints en memoria,
 * midiendo la latencia con clock_gettime(CLOCK_MONOTONIC).
 *
 * ESTO NO ES EL EXPERIMENTO CIENTÍFICO REAL.
 */

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bench.h"
#include "omega/message.h"
#include "omega/endpoint.h"

static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

int pilot1_run_size(uint32_t payload_size,
                    uint32_t iterations,
                    uint32_t warmup,
                    FILE *out) {
    if (payload_size > OMEGA_MSG_MAX_SIZE) return -1;
    if (out == NULL) return -1;

    omega_endpoint_t to_server;
    omega_endpoint_t to_client;
    if (omega_endpoint_init(&to_server) != 0) return -1;
    if (omega_endpoint_init(&to_client) != 0) return -1;

    omega_message_t req, resp;
    omega_message_t recv_req, recv_resp;

    if (omega_message_init(&req, 42, payload_size) != 0) return -1;
    omega_message_fill_pattern(&req);

    if (omega_message_init(&resp, 42, payload_size) != 0) return -1;
    if (omega_message_init(&recv_req, 0, 0) != 0) return -1;
    if (omega_message_init(&recv_resp, 0, 0) != 0) return -1;

    uint32_t total = warmup + iterations;

    for (uint32_t i = 0; i < total; i++) {
        uint64_t t0 = now_ns();

        /* Cliente envía request */
        if (omega_endpoint_send(&to_server, &req) != 0) return -1;

        /* Servidor recibe */
        if (omega_endpoint_recv(&to_server, &recv_req) != 0) return -1;

        /* Servidor prepara response */
        memcpy(resp.payload, recv_req.payload, recv_req.length);
        resp.length = recv_req.length;

        /* Servidor envía response */
        if (omega_endpoint_send(&to_client, &resp) != 0) return -1;

        /* Cliente recibe response */
        if (omega_endpoint_recv(&to_client, &recv_resp) != 0) return -1;

        uint64_t t1 = now_ns();

        /* Solo registrar después del warm-up */
        if (i >= warmup) {
            uint32_t iter = i - warmup;
            fprintf(out, "%u,%u,%llu\n",
                    payload_size,
                    iter,
                    (unsigned long long)(t1 - t0));
        }
    }

    return 0;
}

int pilot1_run_all(const char *output_path) {
    if (output_path == NULL) return -1;

    FILE *out = fopen(output_path, "w");
    if (out == NULL) {
        perror("fopen");
        return -1;
    }

    /* Cabecera CSV */
    fprintf(out, "size,iteration,latency_ns\n");

    uint32_t sizes[PILOT1_NUM_SIZES] = {
        PILOT1_SIZE_8,
        PILOT1_SIZE_64,
        PILOT1_SIZE_512,
        PILOT1_SIZE_4096
    };

    for (int s = 0; s < PILOT1_NUM_SIZES; s++) {
        printf("Ejecutando tamaño %u (%u warmup + %u mediciones)...\n",
               sizes[s], PILOT1_WARMUP, PILOT1_ITERATIONS);
        fflush(stdout);

        if (pilot1_run_size(sizes[s],
                            PILOT1_ITERATIONS,
                            PILOT1_WARMUP,
                            out) != 0) {
            fprintf(stderr, "ERROR: fallo en tamaño %u\n", sizes[s]);
            fclose(out);
            return -1;
        }
    }

    fclose(out);
    return 0;
}
