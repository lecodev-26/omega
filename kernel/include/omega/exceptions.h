#ifndef OMEGA_BOOT_EXCEPTIONS_H
#define OMEGA_BOOT_EXCEPTIONS_H

#include <stdint.h>

void exceptions_init(void);

typedef enum {
    OMEGA_EXC_NONE = 0,
    OMEGA_EXC_SYNC,
    OMEGA_EXC_IRQ,
    OMEGA_EXC_FIQ,
    OMEGA_EXC_SERROR
} omega_exception_class_t;

typedef struct {
    omega_exception_class_t class;
    uint64_t esr_el1;
    uint64_t elr_el1;
    uint64_t far_el1;
    uint64_t spsr_el1;
} omega_exception_info_t;

omega_exception_info_t *exceptions_last_info(void);
void exceptions_print_last(void);

/* Devuelve el contador de IRQs atendidas. */
uint64_t exceptions_irq_count(void);

#endif /* OMEGA_BOOT_EXCEPTIONS_H */
