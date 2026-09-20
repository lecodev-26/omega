/*
 * OMEGA — Kernel
 * mmu.c — Memoria virtual (MMU) para aarch64
 *
 * Mapeo identidad de 2 GB con block descriptors de 2 MB.
 * UART y GIC como Device memory.
 * El kernel está en 0x40000000, dentro del mapeo.
 */

#include "omega/mmu.h"

/*
 * Tablas de traducción.
 *
 * L0: 1 entrada (apunta a L1).
 * L1: 2 entradas (apuntan a L2 low y L2 high).
 * L2 low: 512 entradas de 2 MB (cubre 0x00000000 - 0x3FFFFFFF).
 * L2 high: 512 entradas de 2 MB (cubre 0x40000000 - 0x7FFFFFFF).
 */
static uint64_t g_l0_table[512] __attribute__((aligned(4096)));
static uint64_t g_l1_table[512] __attribute__((aligned(4096)));
static uint64_t g_l2_table_low[512] __attribute__((aligned(4096)));
static uint64_t g_l2_table_high[512] __attribute__((aligned(4096)));

/* Atributos de memoria */
#define MT_NORMAL   0   /* MAIR attr 0: Normal, Write-Back */
#define MT_DEVICE   1   /* MAIR attr 1: Device, nGnRnE */

/* Descriptor de tabla (L0, L1) */
#define DESC_TABLE  (0x3ULL)

/* Descriptor de bloque (L2, 2 MB) */
#define DESC_BLOCK  (0x1ULL)

/* Construir un descriptor de tabla */
static uint64_t make_table_desc(uint64_t addr) {
    return (addr & 0x0000FFFFFFFFF000ULL) | DESC_TABLE;
}

/* Construir un descriptor de bloque de 2 MB */
static uint64_t make_block_desc(uint64_t addr, int attr, int executable) {
    uint64_t desc = (addr & 0x0000FFFFFFE00000ULL) | DESC_BLOCK;

    /* AF = 1 (bit 10) */
    desc |= (1ULL << 10);

    /* SH = 11 (Inner Shareable) (bits 9:8) */
    desc |= (3ULL << 8);

    /* AP = 00 (Read-Write, EL1) (bits 7:6) */
    /* UXN = 1 (no ejecutable en EL0) (bit 54) */
    desc |= (1ULL << 54);

    /* PXN = 0 si executable, 1 si no (bit 53) */
    if (!executable) {
        desc |= (1ULL << 53);
    }

    /* AttrIndx = attr (bits 4:2) */
    desc |= ((uint64_t)attr << 2);

    return desc;
}

void mmu_init(void) {
    /*
     * L0[0] → L1 (cubre VA 0x0000000000000000 - 0x0000007FFFFFFFFF)
     */
    g_l0_table[0] = make_table_desc((uint64_t)g_l1_table);

    /*
     * L1[0] → L2 low (cubre 0x00000000 - 0x3FFFFFFF)
     * L1[1] → L2 high (cubre 0x40000000 - 0x7FFFFFFF)
     */
    g_l1_table[0] = make_table_desc((uint64_t)g_l2_table_low);
    g_l1_table[1] = make_table_desc((uint64_t)g_l2_table_high);

    /*
     * L2 low: 0x00000000 - 0x3FFFFFFF
     *
     * Mapeo:
     *   0x00000000 - 0x07FFFFFF (4 entradas): Normal, ejecutable
     *   0x08000000 - 0x09FFFFFF (1 entrada):  Device (GIC + UART)
     *   0x0A000000 - 0x3FFFFFFF (507 entradas): Normal, ejecutable
     */

    for (int i = 0; i < 4; i++) {
        g_l2_table_low[i] = make_block_desc((uint64_t)(i * 0x200000), MT_NORMAL, 1);
    }
    g_l2_table_low[4] = make_block_desc(0x08000000ULL, MT_DEVICE, 0);
    for (int i = 5; i < 512; i++) {
        g_l2_table_low[i] = make_block_desc((uint64_t)(i * 0x200000), MT_NORMAL, 1);
    }

    /*
     * L2 high: 0x40000000 - 0x7FFFFFFF
     *
     * Todo Normal, ejecutable. El kernel está en 0x40000000.
     */
    for (int i = 0; i < 512; i++) {
        g_l2_table_high[i] = make_block_desc(0x40000000ULL + (uint64_t)(i * 0x200000), MT_NORMAL, 1);
    }
}

void mmu_enable(void) {
    uint64_t mair = 0xFFULL | (0x00ULL << 8);
    __asm__ volatile("msr mair_el1, %0" :: "r"(mair));

    uint64_t tcr = 0;
    tcr |= (16ULL << 0);
    tcr |= (1ULL << 8);
    tcr |= (1ULL << 10);
    tcr |= (3ULL << 12);
    tcr |= (0ULL << 14);
    tcr |= (16ULL << 16);
    tcr |= (2ULL << 30);
    tcr |= (5ULL << 32);
    __asm__ volatile("msr tcr_el1, %0" :: "r"(tcr));

    uint64_t ttbr0 = (uint64_t)g_l0_table;
    __asm__ volatile("msr ttbr0_el1, %0" :: "r"(ttbr0));

    __asm__ volatile("dsb ish");
    __asm__ volatile("isb");

    __asm__ volatile("tlbi vmalle1");
    __asm__ volatile("dsb ish");
    __asm__ volatile("isb");

    uint64_t sctlr;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1ULL << 0);
    __asm__ volatile("msr sctlr_el1, %0" :: "r"(sctlr));
    __asm__ volatile("isb");
}
