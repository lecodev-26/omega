/*
 * OMEGA — Boot minimal
 * boot.S
 *
 * Punto de entrada bare-metal para aarch64.
 * Configura el stack y salta a kmain.
 */

.section .text.boot
.global _start

_start:
    /* Deshabilitar interrupciones */
    msr daifset, #0xf

    /* Configurar stack */
    ldr x0, =__stack_top
    mov sp, x0

    /* Saltar a kmain */
    bl kmain

    /* Si kmain retorna, detener la CPU */
1:
    wfe
    b 1b
