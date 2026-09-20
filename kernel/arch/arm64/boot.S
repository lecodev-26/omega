/*
 * OMEGA — Boot
 * boot.S — Punto de entrada, drop a EL1, init MMU
 */

.section .text.boot
.global _start

_start:
    /* Deshabilitar interrupciones */
    msr daifset, #0xf

    /*
     * Leer CurrentEL. Si estamos en EL2 o superior, bajar a EL1.
     */
    mrs x0, CurrentEL
    lsr x0, x0, #2
    and x0, x0, #0x3
    cmp x0, #0x1
    beq el1_entry

    /* Si estamos en EL2, configurar y bajar a EL1 */
    mov x0, #(1 << 31)
    msr hcr_el2, x0

    mov x0, #0x5
    msr spsr_el2, x0

    ldr x0, =el1_entry
    msr elr_el2, x0

    eret

el1_entry:
    /* Configurar stack */
    ldr x0, =__stack_top
    mov sp, x0

    /* Inicializar y habilitar MMU */
    bl mmu_init
    bl mmu_enable

    /* Saltar a kmain */
    bl kmain

    /* Si kmain retorna, detener la CPU */
1:
    wfe
    b 1b
