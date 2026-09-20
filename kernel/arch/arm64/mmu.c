/*
 * OMEGA — Kernel
 * mmu.c — Memoria virtual (MMU) para aarch64
 *
 * Mapeo identidad de 2 GB con block descriptors de 2 MB.
 * UART y GIC como Device memory.
 *
 * NOTA: data cache (C) e instruction cache (I) deshabilitadas.
 * Pendiente investigar por qué el I-cache causa deadlock.
 */

#include "omega/mmu.h"

static uint64_t g_l0_table[512] __attribute__((aligned(4096)));
static uint64_t g_l1_table[512] __attribute__((aligned(4096)));
static uint64_t g_l2_table_low[512] __attribute__((aligned(4096)));
static uint64_t g_l2_table_high[512] __attribute__((aligned(4096)));

#define MT_NORMAL   0
#define MT_DEVICE   1

#define DESC_TABLE  (0x3ULL)
#define DESC_BLOCK  (0x1ULL)

static uint64_t make_table_desc(uint64_t addr) {
    return (addr & 0x0000FFFFFFFFF000ULL) | DESC_TABLE;
}

static uint64_t make_block_desc(uint64_t addr, int attr, int executable) {
    uint64_t desc = (addr & 0x0000FFFFFFE00000ULL) | DESC_BLOCK;
    desc |= (1ULL << 10);   /* AF */
    desc |= (3ULL << 8);    /* SH */
    desc |= (1ULL << 54);   /* UXN */
    if (!executable) {
        desc |= (1ULL << 53);   /* PXN */
    }
    desc |= ((uint64_t)attr << 2);
    return desc;
}

void mmu_init(void) {
    g_l0_table[0] = make_table_desc((uint64_t)g_l1_table);
    g_l1_table[0] = make_table_desc((uint64_t)g_l2_table_low);
    g_l1_table[1] = make_table_desc((uint64_t)g_l2_table_high);

    for (int i = 0; i < 4; i++) {
        g_l2_table_low[i] = make_block_desc((uint64_t)(i * 0x200000), MT_NORMAL, 1);
    }
    g_l2_table_low[4] = make_block_desc(0x08000000ULL, MT_DEVICE, 0);
    for (int i = 5; i < 512; i++) {
        g_l2_table_low[i] = make_block_desc((uint64_t)(i * 0x200000), MT_NORMAL, 1);
    }

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

    /*
     * SCTLR_EL1:
     *   M = 1 (MMU enable)
     *   C = 0 (data cache disabled)
     *   I = 0 (instruction cache disabled)
     */
    uint64_t sctlr;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= (1ULL << 0);    /* M */
    sctlr &= ~(1ULL << 2);   /* C = 0 */
    sctlr &= ~(1ULL << 12);  /* I = 0 */
    __asm__ volatile("msr sctlr_el1, %0" :: "r"(sctlr));
    __asm__ volatile("isb");
}
