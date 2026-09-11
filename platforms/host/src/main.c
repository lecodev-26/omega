/*
 * OMEGA — Host prototype
 * main.c (v2)
 */

#include <stdio.h>
#include <string.h>
#include "omega/message.h"
#include "omega/endpoint.h"
#include "omega/capability.h"

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
    if (omega_capability_init(&root, 1, /*owner*/100,
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
    if (omega_capability_delegate(&root, /*new owner*/200, &delegated) != 0) {
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

int main(void) {
    printf("=== OMEGA Host Prototype v2 ===\n");
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

    printf("\n=== Fin ===\n");
    return 0;
}
