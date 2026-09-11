#ifndef OMEGA_BOOT_TIMER_H
#define OMEGA_BOOT_TIMER_H

#include <stdint.h>

uint64_t timer_frequency(void);
uint64_t timer_read_counter(void);
void timer_set_compare(uint64_t future_ticks);
void timer_enable(void);
void timer_disable(void);
void timer_delay_us(uint64_t us);

/*
 * Programa el timer para disparar una IRQ en el futuro.
 * El delta está en ticks del contador físico.
 */
void timer_schedule_irq(uint64_t delta_ticks);

/*
 * Handler llamado desde el exception handler cuando llega TIMER_IRQ.
 * Rearma el timer y actualiza contadores internos.
 */
void timer_irq_handler(void);

/*
 * Devuelve el número de ticks del timer que han ocurrido.
 */
uint64_t timer_irq_count(void);

#endif /* OMEGA_BOOT_TIMER_H */
