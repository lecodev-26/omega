#include "gic.h"
#include "uart.h"

#define GICD_CTLR        0x0000
#define GICD_TYPER       0x0004
#define GICD_ISENABLER   0x0100
#define GICD_IPRIORITYR  0x0400
#define GICD_ITARGETSR   0x0800
#define GICD_IGROUPR     0x0080

#define GICC_CTLR        0x0000
#define GICC_PMR         0x0004
#define GICC_IAR         0x000C
#define GICC_EOIR        0x0010
#define GICC_HPPIR       0x0018

static inline void mmio_write32(uintptr_t addr, uint32_t value) {
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t mmio_read32(uintptr_t addr) {
    return *(volatile uint32_t *)addr;
}

void gic_init(void) {
    uart_puts("[gic] GICD_TYPER = ");
    uart_puthex64(mmio_read32(GICD_BASE + GICD_TYPER));
    uart_puts("\n");

    /* 1. Configurar el grupo de la IRQ 30 como Group 0 */
    /* GICD_IGROUPR[n/32], bit (n%32). En GICv2, bit=0 → Group 0 */
    /* Los PPI (16..31) suelen estar hardwired a Group 1, pero probemos */
    mmio_write32(GICD_BASE + GICD_IGROUPR, 0x00000000);

    /* 2. Habilitar el GIC Distributor */
    mmio_write32(GICD_BASE + GICD_CTLR, 1);
    uart_puts("[gic] GICD_CTLR = ");
    uart_puthex64(mmio_read32(GICD_BASE + GICD_CTLR));
    uart_puts("\n");

    /* 3. Configurar la CPU interface */
    mmio_write32(GICC_BASE + GICC_PMR, 0xFF);
    uart_puts("[gic] GICC_PMR = ");
    uart_puthex64(mmio_read32(GICC_BASE + GICC_PMR));
    uart_puts("\n");

    mmio_write32(GICC_BASE + GICC_CTLR, 1);
    uart_puts("[gic] GICC_CTLR = ");
    uart_puthex64(mmio_read32(GICC_BASE + GICC_CTLR));
    uart_puts("\n");

    /* 4. Prioridad y CPU objetivo para la IRQ 30 */
    mmio_write32(GICD_BASE + GICD_IPRIORITYR + 28, 0x00000000);
    mmio_write32(GICD_BASE + GICD_ITARGETSR + 28, 0x01010101);
}

void gic_enable_irq(uint32_t irq) {
    uint32_t reg = irq / 32;
    uint32_t bit = irq % 32;
    mmio_write32(GICD_BASE + GICD_ISENABLER + reg * 4, (uint32_t)(1u << bit));

    /* Verificar */
    uart_puts("[gic] ISENABLER[0] = ");
    uart_puthex64(mmio_read32(GICD_BASE + GICD_ISENABLER));
    uart_puts("\n");
}

uint32_t gic_acknowledge(void) {
    return mmio_read32(GICC_BASE + GICC_IAR);
}

void gic_eoi(uint32_t irq) {
    mmio_write32(GICC_BASE + GICC_EOIR, irq);
}

void gic_enable_cpu_irqs(void) {
    __asm__ volatile("msr daifclr, #2");
    __asm__ volatile("isb");
}

void gic_disable_cpu_irqs(void) {
    __asm__ volatile("msr daifset, #2");
    __asm__ volatile("isb");
}
