# OMEGA — Kernel — ARM64

Código específico de la arquitectura ARM64 (aarch64).

## Archivos

- `boot.S` — Punto de entrada `_start`. Configura stack y salta a `kmain`.
- `exceptions_asm.S` — Vector table + stubs de excepción.
- `exceptions.c` — Handler C de excepciones.
- `gic.c` — Driver del GIC (Generic Interrupt Controller).
- `timer.c` — Driver del ARM Generic Timer.
- `uart.c` — Driver del UART PL011 (QEMU virt).

## Convenciones

- Toda función externa (llamable desde C) es `void` o `uint64_t` (evitar
  floats por `-mgeneral-regs-only`).
- Los handlers de excepción guardan x0-x30 completo.
- Se usa `eret` para retornar de IRQs recuperables.

## Notas

- QEMU virt con `-cpu cortex-a53` arranca en EL1.
- El UART está en `0x09000000` (PL011).
- El GIC (GICv2) está en `GICD=0x08000000`, `GICC=0x08010000`.
- El PPI del timer físico es 30.
