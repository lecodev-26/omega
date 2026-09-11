#include <assert.h>
#include <stdio.h>
#include "omega/endpoint.h"

int main(void) {
    omega_endpoint_t ep;
    omega_message_t msg;
    omega_message_t out;

    assert(omega_endpoint_init(&ep) == 0);
    assert(omega_endpoint_count(&ep) == 0);

    /* Recibir de cola vacía falla */
    assert(omega_endpoint_recv(&ep, &out) == -1);

    /* Enviar y recibir */
    assert(omega_message_init(&msg, 1, 8) == 0);
    omega_message_fill_pattern(&msg);
    assert(omega_endpoint_send(&ep, &msg) == 0);
    assert(omega_endpoint_count(&ep) == 1);

    assert(omega_endpoint_recv(&ep, &out) == 0);
    assert(omega_endpoint_count(&ep) == 0);
    assert(omega_message_verify_pattern(&out) == 0);

    printf("test_endpoint: OK\n");
    return 0;
}
