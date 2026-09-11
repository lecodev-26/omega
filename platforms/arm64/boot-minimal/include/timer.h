/*
 * OMEGA — Boot minimal
 * timer.h — ARM Generic Timer
 *
 * El ARM Generic Timer proporciona:
 *   - CNTPCT_EL0: contador físico (64 bits)
 *   - CNTFRQ_EL0: frecuencia del contador (Hz)
 *   - CNTP_TVAL_EL0: valor del timer físico (para comparar)
 *   - CNTP_CTL_EL0: control del timer físico
 *
 * En QEMU virt, el timer está conectado a la IRQ 30 del GIC (no configurado aquí).
 * Para este banco de pruebas, usamos POLLING en lugar de IRQ real.
 */

#ifndef OMEGA_BOOT_TIMER_H
#define OMEGA_BOOT_TIMER_H

#include <stdint.h>

/*
 * Devuelve la frecuencia del contador físico (Hz).
 * En QEMU virt, típicamente 62.5 MHz (62500000 Hz).
 */
uint64_t timer_frequency(void);

/*
 * Devuelve el valor actual del contador físico.
 */
uint64_t timer_read_counter(void);

/*
 * Configura el timer físico para disparar en un futuro absoluto.
 * (No se usa IRQ todavía; esto es preparación.)
 */
void timer_set_compare(uint64_t future_ticks);

/*
 * Habilita el timer físico.
 */
void timer_enable(void);

/*
 * Deshabilita el timer físico.
 */
void timer_disable(void);

/*
 * Espera N microsegundos usando el contador físico (busy-wait).
 * Útil para delays sin IRQ.
 */
void timer_delay_us(uint64_t us);

#endif /* OMEGA_BOOT_TIMER_H */
