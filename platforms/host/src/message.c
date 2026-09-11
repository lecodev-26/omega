/*
 * OMEGA — Host prototype
 * message.c
 */

#include "omega/message.h"
#include <string.h>

int omega_message_init(omega_message_t *msg, uint32_t id, uint32_t length) {
    if (msg == NULL) return -1;
    if (length > OMEGA_MSG_MAX_SIZE) return -1;

    memset(msg, 0, sizeof(*msg));
    msg->id = id;
    msg->length = length;
    return 0;
}

void omega_message_fill_pattern(omega_message_t *msg) {
    if (msg == NULL) return;
    for (uint32_t i = 0; i < msg->length; i++) {
        msg->payload[i] = (uint8_t)((msg->id + i) & 0xFFu);
    }
}

int omega_message_verify_pattern(const omega_message_t *msg) {
    if (msg == NULL) return -1;
    for (uint32_t i = 0; i < msg->length; i++) {
        uint8_t expected = (uint8_t)((msg->id + i) & 0xFFu);
        if (msg->payload[i] != expected) return -1;
    }
    return 0;
}
