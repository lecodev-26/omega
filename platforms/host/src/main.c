/*
 * OMEGA — Host prototype
 * main.c (v3)
 */

#include <stdio.h>
#include <string.h>
#include "omega/message.h"
#include "omega/endpoint.h"
#include "omega/capability.h"
#include "omega/service.h"

static void run_transaction(omega_endpoint_t *to_server,
                            omega_endpoint_t *to_client,
                            uint32_t payload_size) {
    omega_message_t request;
    omega_message_t response;

    if (omega_message_init(&request, 42, payload_size) != 0) {
        printf("ERROR: no se pudo inicializar request\n");
        return;
    }
    omega_message_fill_pattern(&request);

    if (omega_endpoint_send(to_server, &request) != 0) {
        printf("ERROR: no se pudo enviar request\n");
        return;
    }

    omega_message_t received;
    if (omega_endpoint_recv(to_server, &received) != 0) {
        printf("ERROR: servidor no recibió request\n");
        return;
    }

    if (omega_message_verify_pattern(&received) != 0) {
        printf("ERROR: payload corrupto en request\n");
        return;
    }

    if (omega_message_init(&response, received.id, received.length) != 0) {
        printf("ERROR: no se pudo inicializar response\n");
        return;
    }
    memcpy(response.payload, received.payload, received.length);

    if (omega_endpoint_send(to_client, &response) != 0) {
        printf("ERROR: no se pudo enviar response\n");
        return;
    }

    omega_message_t client_response;
    if (omega_endpoint_recv(to_client, &client_response) != 0) {
        printf("ERROR: cliente no recibió response\n");
        return;
    }

    if (omega_message_verify_pattern(&client_response) != 0) {
        printf("ERROR: payload corrupto en response\n");
        return;
    }

    printf("OK: transacción completada con payload de %u bytes\n",
           (unsigned)payload_size);
}

static void demo_capabilities(void) {
    printf("\n--- Capabilities v2 (con delegación) ---\n");

    omega_capability_t root;
    if (omega_capability_init(&root, 1, 100,
        OMEGA_CAP_RIGHT_READ | OMEGA_CAP_RIGHT_WRITE | OMEGA_CAP_RIGHT_GRANT) != 0) {
        return;
    }
    printf("Root: object=%u owner=%u rights=0x%X\n",
           root.object_id, root.owner_id, root.rights);

    omega_capability_t child;
    if (omega_capability_derive(&root, &child,
        OMEGA_CAP_RIGHT_READ | OMEGA_CAP_RIGHT_GRANT) != 0) {
        printf("ERROR: no se pudo derivar\n");
        return;
    }
    printf("Child: object=%u owner=%u rights=0x%X\n",
           child.object_id, child.owner_id, child.rights);

    omega_capability_t delegated;
    if (omega_capability_delegate(&root, 200, &delegated) != 0) {
        printf("ERROR: no se pudo delegar\n");
        return;
    }
    printf("Delegated: object=%u owner=%u rights=0x%X parent_owner=%u\n",
           delegated.object_id, delegated.owner_id,
           delegated.rights, delegated.parent_owner);

    if (!omega_capability_has(&delegated, OMEGA_CAP_RIGHT_GRANT)) {
        printf("Delegated NO tiene GRANT (correcto: no se propaga)\n");
    }
}

static void demo_services(void) {
    printf("\n--- Services ---\n");

    omega_service_t echo_svc;
    if (omega_service_init(&echo_svc, OMEGA_SERVICE_ECHO, "echo",
        omega_service_handler_echo) != 0) {
        printf("ERROR: no se pudo crear echo service\n");
        return;
    }

    omega_service_t time_svc;
    if (omega_service_init(&time_svc, OMEGA_SERVICE_TIME, "time",
        omega_service_handler_time) != 0) {
        printf("ERROR: no se pudo crear time service\n");
        return;
    }

    /* Cliente envía request al echo service */
    omega_message_t req;
    if (omega_message_init(&req, 7, 64) != 0) return;
    omega_message_fill_pattern(&req);

    if (omega_endpoint_send(&echo_svc.inbox, &req) != 0) {
        printf("ERROR: no se pudo enviar al echo service\n");
        return;
    }

    /* Servicio procesa */
    omega_message_t resp;
    if (omega_service_step(&echo_svc, &resp) != 0) {
        printf("ERROR: echo service no procesó\n");
        return;
    }

    if (omega_message_verify_pattern(&resp) != 0) {
        printf("ERROR: respuesta del echo service corrupta\n");
        return;
    }
    printf("Echo service: OK (payload=%u bytes)\n", (unsigned)resp.length);

    /* Time service: dos invocaciones */
    omega_message_t t_req;
    if (omega_message_init(&t_req, 8, 8) != 0) return;

    omega_message_t t_resp1, t_resp2;
    if (omega_service_handler_time(&t_req, &t_resp1) != 0) return;
    if (omega_service_handler_time(&t_req, &t_resp2) != 0) return;

    uint32_t t1 = 0, t2 = 0;
    memcpy(&t1, t_resp1.payload, sizeof(uint32_t));
    memcpy(&t2, t_resp2.payload, sizeof(uint32_t));
    printf("Time service: t1=%u t2=%u (monótono: %s)\n",
           t1, t2, (t2 > t1) ? "sí" : "no");
}

int main(void) {
    printf("=== OMEGA Host Prototype v3 ===\n");
    printf("Esto NO es el kernel de OMEGA. Es un banco de pruebas conceptual.\n\n");

    omega_endpoint_t to_server;
    omega_endpoint_t to_client;
    if (omega_endpoint_init(&to_server) != 0) return 1;
    if (omega_endpoint_init(&to_client) != 0) return 1;

    printf("--- Transacciones request-response ---\n");
    run_transaction(&to_server, &to_client, OMEGA_MSG_SIZE_8);
    run_transaction(&to_server, &to_client, OMEGA_MSG_SIZE_64);
    run_transaction(&to_server, &to_client, OMEGA_MSG_SIZE_512);
    run_transaction(&to_server, &to_client, OMEGA_MSG_SIZE_4096);

    demo_capabilities();
    demo_services();

    printf("\n=== Fin ===\n");
    return 0;
}
