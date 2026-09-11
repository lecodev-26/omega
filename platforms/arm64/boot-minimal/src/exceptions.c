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
    switch (class) {
        case OMEGA_EXC_SYNC:   uart_puts("  class: SYNC\n");   break;
        case OMEGA_EXC_IRQ:    uart_puts("  class: IRQ\n");    break;
        case OMEGA_EXC_FIQ:    uart_puts("  class: FIQ\n");    break;
        case OMEGA_EXC_SERROR: uart_puts("  class: SERROR\n"); break;
        default:               uart_puts("  class: ?\n");      break;
    }
}
