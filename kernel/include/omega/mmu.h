/*
 * OMEGA — Kernel
 * mmu.h — Memoria virtual (MMU) para aarch64
 */

#ifndef OMEGA_MMU_H
#define OMEGA_MMU_H

#include <stdint.h>

/*
 * Inicializa las tablas de traducción con un mapeo identidad.
 *
 * Mapea 1 GB (0x00000000 - 0x3FFFFFFF) con block descriptors de 2 MB.
 * La UART y el GIC se mapean como Device memory.
 */
void mmu_init(void);

/*
 * Habilita la MMU.
 *
 * Configura MAIR_EL1, TCR_EL1, TTBR0_EL1 y SCTLR_EL1.
 */
void mmu_enable(void);

#endif /* OMEGA_MMU_H */
