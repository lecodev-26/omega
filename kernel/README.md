# OMEGA — Kernel (prototipo)

**ESTADO: PROTOTIPO.**

Este directorio contiene el **prototipo del kernel de OMEGA**.
No es un kernel completo, pero es la base sobre la que se construirá.

## Qué hace

- Arranca en bare-metal (aarch64) desde `_start`.
- Configura stack, UART, vector table, GIC, ARM Generic Timer.
- Captura excepciones síncronas y las imprime.
- Recibe IRQs del timer.
- Salida controlada por UART (`q` para salir).

## Qué NO hace

- No gestiona memoria virtual (sin MMU).
- No tiene scheduler.
- No tiene IPC.
- No tiene capabilities.
- No tiene user space.
- No tiene drivers más allá del UART.

## Estructura

```

kernel/
├── README.md
├── Makefile
├── linker.ld
├── include/
│   └── omega/
│       ├── uart.h
│       ├── gic.h
│       ├── timer.h
│       └── exceptions.h
├── arch/
│   └── arm64/
│       ├── README.md
│       ├── boot.S            (entry point)
│       ├── exceptions_asm.S  (vector table)
│       ├── exceptions.c
│       ├── gic.c
│       ├── timer.c
│       └── uart.c
├── kmain.c                   (punto de entrada C)
└── build/                    (generado)

```

## Compilar

```bash
cd kernel
make clean
make CC=clang
```

Ejecutar en QEMU

```bash
make run
```

Salida esperada:

```
OMEGA boot minimal v5.1 (IRQ debug)
...
Bucle de polling. Imprime estado cada ~10M nops.
*** Primeros 5 ticks alcanzados ***
---
Ticks recibidos: 5
IRQs totales: 5
=== fin ===
```

Escribe q para salir.

Reglas

· No usar FP/SIMD (-mgeneral-regs-only): acceso a FP/SIMD está
  deshabilitado por defecto en bare-metal aarch64.
· Guardar contexto completo (x0-x30) en handlers de excepción.
· Usar eret para retornar de excepciones recuperables.
· No afirmar que algo funciona sin probarlo.

Roadmap del kernel

1. ✅ Boot en QEMU
2. ✅ UART
3. ✅ Vector table + excepciones
4. ✅ ARM Generic Timer (polling)
5. ✅ GIC + IRQ real
6. ⏳ Scheduler
7. ⏳ MMU + memoria virtual
8. ⏳ IPC
9. ⏳ User space
10. ⏳ Drivers
    EOF

echo "kernel/README.md creado"
