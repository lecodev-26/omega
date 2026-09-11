/*
 * OMEGA — Boot minimal
 * gic.h — GICv2 (QEMU virt)
 */

#ifndef OMEGA_BOOT_GIC_H
#define OMEGA_BOOT_GIC_H

#include <stdint.h>

/* Direcciones del GICv2 en QEMU virt */
#define GICD_BASE  0x08000000
#define GICC_BASE  0x08010000

/* PPI del timer físico en QEMU virt */
#define TIMER_IRQ  30

/*
 * Inicializa el GIC:
 *   - Habilita el distributor.
 *   - Habilita la CPU interface.
 *   - Configura la máscara de prioridad.
 */
void gic_init(void);

/*
 * Habilita una IRQ concreta (PPI o SPI).
 */
void gic_enable_irq(uint32_t irq);

/*
 * Lee el IAR (acknowledge). Devuelve el IRQ ID que se está atendiendo.
 */
uint32_t gic_acknowledge(void);

/*
 * Escribe el EOI (End Of Interrupt).
 */
void gic_eoi(uint32_t irq);

/*
 * Habilita las interrupciones en el PSTATE (DAIF.I = 0).
 */
void gic_enable_cpu_irqs(void);

/*
 * Deshabilita las interrupciones en el PSTATE (DAIF.I = 1).
 */
void gic_disable_cpu_irqs(void);

#endif /* OMEGA_BOOT_GIC_H */
