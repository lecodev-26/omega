/*
 * OMEGA — Boot minimal
 * exceptions.h — Manejo de excepciones aarch64
 *
 * Este es un PROTOTIPO CONCEPTUAL. No es el kernel de OMEGA.
 */

#ifndef OMEGA_BOOT_EXCEPTIONS_H
#define OMEGA_BOOT_EXCEPTIONS_H

#include <stdint.h>

/*
 * Inicializa el vector table.
 * Instala la dirección de la vector table en VBAR_EL1.
 */
void exceptions_init(void);

/*
 * Clase de excepción capturada.
 * Se rellena por los handlers y se consulta desde C.
 */
typedef enum {
    OMEGA_EXC_NONE = 0,
    OMEGA_EXC_SYNC,
    OMEGA_EXC_IRQ,
    OMEGA_EXC_FIQ,
    OMEGA_EXC_SERROR
} omega_exception_class_t;

/*
 * Información de la última excepción capturada.
 */
typedef struct {
    omega_exception_class_t class;
    uint64_t esr_el1;    /* Exception Syndrome Register */
    uint64_t elr_el1;    /* Exception Link Register (PC) */
    uint64_t far_el1;    /* Fault Address Register */
    uint64_t spsr_el1;   /* Saved Program Status Register */
} omega_exception_info_t;

/*
 * Devuelve un puntero a la información de la última excepción.
 * Se rellena cuando un handler C es invocado desde assembly.
 */
omega_exception_info_t *exceptions_last_info(void);

/*
 * Imprime la información de la última excepción por UART.
 * (Depende de uart.h.)
 */
void exceptions_print_last(void);

#endif /* OMEGA_BOOT_EXCEPTIONS_H */
