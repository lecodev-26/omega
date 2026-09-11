/*
 * OMEGA — Boot minimal
 * boot.S (v2 — drop to EL1)
 */

.section .text.boot
.global _start

_start:
    /* Deshabilitar interrupciones */
    msr daifset, #0xf

    /*
     * Leer CurrentEL. Si estamos en EL2 o superior, bajar a EL1.
     * Bits [3:2] del registro CurrentEL indican el EL actual.
     */
    mrs x0, CurrentEL
    lsr x0, x0, #2
    and x0, x0, #0x3
    cmp x0, #0x1        /* ¿EL1? */
    beq el1_entry       /* Ya estamos en EL1 */

    /* Si estamos en EL2, configurar y bajar a EL1 */
    /* Configurar HCR_EL2: RW=1 (EL1 ejecuta en AArch64) */
    mov x0, #(1 << 31)
    msr hcr_el2, x0

    /* Configurar SPSR_EL2 para EL1h (usando SP_EL1) */
    mov x0, #0x5        /* M = EL1h (0b0101) */
    msr spsr_el2, x0

    /* Configurar ELR_EL2 con la dirección de el1_entry */
    ldr x0, =el1_entry
    msr elr_el2, x0

    /* Ejecutar eret para bajar a EL1 */
    eret

el1_entry:
    /* Configurar stack */
    ldr x0, =__stack_top
    mov sp, x0

    /* Saltar a kmain */
    bl kmain

    /* Si kmain retorna, detener la CPU */
1:
    wfe
    b 1b
