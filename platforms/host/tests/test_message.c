#include <assert.h>
#include <stdio.h>
#include "omega/message.h"

int main(void) {
    omega_message_t msg;

    /* Init correcto */
    assert(omega_message_init(&msg, 7, 64) == 0);
    assert(msg.id == 7);
    assert(msg.length == 64);

    /* Init con tamaño excesivo */
    assert(omega_message_init(&msg, 7, OMEGA_MSG_MAX_SIZE + 1) == -1);

    /* Patrón */
    assert(omega_message_init(&msg, 7, 64) == 0);
    omega_message_fill_pattern(&msg);
    assert(omega_message_verify_pattern(&msg) == 0);

    /* Corrupción detectada */
    msg.payload[10] ^= 0xFF;
    assert(omega_message_verify_pattern(&msg) == -1);

    printf("test_message: OK\n");
    return 0;
}
