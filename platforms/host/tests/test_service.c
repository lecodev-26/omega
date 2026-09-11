#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "omega/service.h"

static void test_echo(void) {
    omega_service_t svc;
    assert(omega_service_init(&svc, OMEGA_SERVICE_ECHO, "echo",
        omega_service_handler_echo) == 0);

    omega_message_t req;
    assert(omega_message_init(&req, 1, 64) == 0);
    omega_message_fill_pattern(&req);

    assert(omega_endpoint_send(&svc.inbox, &req) == 0);
    assert(omega_endpoint_count(&svc.inbox) == 1);

    assert(omega_service_step(&svc) == 0);
    assert(omega_endpoint_count(&svc.inbox) == 0);

    /* Verificar handler directamente */
    omega_message_t resp;
    assert(omega_message_init(&resp, 1, 64) == 0);
    assert(omega_service_handler_echo(&req, &resp) == 0);
    assert(resp.length == req.length);
    assert(memcmp(resp.payload, req.payload, req.length) == 0);

    printf("test_service (echo): OK\n");
}

static void test_time(void) {
    omega_service_t svc;
    assert(omega_service_init(&svc, OMEGA_SERVICE_TIME, "time",
        omega_service_handler_time) == 0);

    omega_message_t req;
    assert(omega_message_init(&req, 1, 8) == 0);

    omega_message_t resp1, resp2;
    assert(omega_message_init(&resp1, 1, 8) == 0);
    assert(omega_message_init(&resp2, 1, 8) == 0);

    assert(omega_service_handler_time(&req, &resp1) == 0);
    assert(omega_service_handler_time(&req, &resp2) == 0);

    uint32_t t1 = 0, t2 = 0;
    memcpy(&t1, resp1.payload, sizeof(uint32_t));
    memcpy(&t2, resp2.payload, sizeof(uint32_t));

    assert(t2 > t1);

    printf("test_service (time): OK\n");
}

int main(void) {
    test_echo();
    test_time();
    printf("test_service: OK\n");
    return 0;
}
