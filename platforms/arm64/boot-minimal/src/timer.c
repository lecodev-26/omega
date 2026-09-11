#include "timer.h"

#define CNTFRQ_EL0      "cntfrq_el0"
#define CNTPCT_EL0      "cntpct_el0"
#define CNTP_TVAL_EL0   "cntp_tval_el0"
#define CNTP_CTL_EL0    "cntp_ctl_el0"

static uint64_t g_irq_count = 0;

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

uint64_t timer_frequency(void) { return read_cntfrq(); }
uint64_t timer_read_counter(void) { return read_cntpct(); }

void timer_set_compare(uint64_t future_ticks) {
    write_cntp_tval(future_ticks);
}

void timer_enable(void) {
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

void timer_schedule_irq(uint64_t delta_ticks) {
    /* Escribir CNTP_TVAL_EL0 reinicia el contador de comparación
     * con el delta dado, y si el timer está habilitado, dispara
     * una IRQ cuando expira. */
    write_cntp_tval(delta_ticks);
}

void timer_irq_handler(void) {
    g_irq_count++;
    /* Rearmar el timer para el siguiente tick */
    /* Programamos 10 ms = 0.01s * freq = 625000 ticks para 62.5MHz */
    uint64_t freq = timer_frequency();
    uint64_t delta = freq / 100;  /* 10 ms */
    write_cntp_tval(delta);
}

uint64_t timer_irq_count(void) {
    return g_irq_count;
}
