#include "exceptions.h"
#include "uart.h"

static omega_exception_info_t g_last_info;

extern void exceptions_install(void);

void exceptions_init(void) {
    g_last_info.class = OMEGA_EXC_NONE;
    g_last_info.esr_el1 = 0;
    g_last_info.elr_el1 = 0;
    g_last_info.far_el1 = 0;
    g_last_info.spsr_el1 = 0;
    exceptions_install();
}

omega_exception_info_t *exceptions_last_info(void) {
    return &g_last_info;
}

void exception_handler_c(omega_exception_class_t class,
                         uint64_t esr_el1,
                         uint64_t elr_el1,
                         uint64_t far_el1,
                         uint64_t spsr_el1) {
    g_last_info.class = class;
    g_last_info.esr_el1 = esr_el1;
    g_last_info.elr_el1 = elr_el1;
    g_last_info.far_el1 = far_el1;
    g_last_info.spsr_el1 = spsr_el1;

    uart_puts("\n[EXCEPTION]\n");
    uart_puts("  class: ");
    switch (class) {
        case OMEGA_EXC_SYNC:   uart_puts("SYNC\n");   break;
        case OMEGA_EXC_IRQ:    uart_puts("IRQ\n");    break;
        case OMEGA_EXC_FIQ:    uart_puts("FIQ\n");    break;
        case OMEGA_EXC_SERROR: uart_puts("SERROR\n"); break;
        default:               uart_puts("?\n");      break;
    }
    uart_puts("  ESR_EL1:  "); uart_puthex64(esr_el1);  uart_puts("\n");
    uart_puts("  ELR_EL1:  "); uart_puthex64(elr_el1);  uart_puts("\n");
    uart_puts("  FAR_EL1:  "); uart_puthex64(far_el1);  uart_puts("\n");
    uart_puts("  SPSR_EL1: "); uart_puthex64(spsr_el1); uart_puts("\n");
    uart_puts("[fin excepcion]\n");
}

void exceptions_print_last(void) {
    omega_exception_info_t *info = &g_last_info;
    if (info->class == OMEGA_EXC_NONE) {
        uart_puts("No hay excepciones registradas.\n");
        return;
    }
    uart_puts("Ultima excepcion registrada.\n");
}
