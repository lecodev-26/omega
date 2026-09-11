/*
 * OMEGA — Boot minimal
 * timer.c
 *
 * ARM Generic Timer — implementación mínima para QEMU virt.
 *
 * NOTA: No configuramos el GIC todavía. Este timer solo puede
 * consultarse por polling. La IRQ real requiere configurar el GIC.
 */

#include "timer.h"

/* Registros del ARM Generic Timer (acceso desde EL1) */
#define CNTFRQ_EL0      "cntfrq_el0"
#define CNTPCT_EL0      "cntpct_el0"
#define CNTP_TVAL_EL0   "cntp_tval_el0"
#define CNTP_CTL_EL0    "cntp_ctl_el0"

static inline uint64_t read_cntfrq(void) {
    uint64_t v;
    __asm__ volatile("mrs %0, " CNTFRQ_EL0 : "=r"(v));
    return v;
}

static inline uint64_t read_cntpct(void) {
    uint64_t v;
    __asm__ volatile("mrs %0, " CNTPCT_EL0 : "=r"(v));
    return v;
}

static inline void write_cntp_tval(uint64_t v) {
    __asm__ volatile("msr " CNTP_TVAL_EL0 ", %0" :: "r"(v));
}

static inline void write_cntp_ctl(uint64_t v) {
    __asm__ volatile("msr " CNTP_CTL_EL0 ", %0" :: "r"(v));
    __asm__ volatile("isb");
}

uint64_t timer_frequency(void) {
    return read_cntfrq();
}

uint64_t timer_read_counter(void) {
    return read_cntpct();
}

void timer_set_compare(uint64_t future_ticks) {
    /* CNTP_TVAL_EL0 es relativo: escribe el delta en ticks.
     * Escribir aquí reinicia el contador de comparación. */
    write_cntp_tval(future_ticks);
}

void timer_enable(void) {
    /* CNTP_CTL_EL0: bit 0 = ENABLE, bit 1 = IMASK, bit 2 = ISTATUS */
    write_cntp_ctl(1);
}

void timer_disable(void) {
    write_cntp_ctl(0);
}

void timer_delay_us(uint64_t us) {
    uint64_t freq = timer_frequency();
    uint64_t ticks = (freq / 1000000ULL) * us;
    uint64_t start = timer_read_counter();
    while ((timer_read_counter() - start) < ticks) {
        __asm__ volatile("nop");
    }
}
