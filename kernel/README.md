# OMEGA — Kernel (prototipo)

**ESTADO: PROTOTIPO FUNCIONAL CON PREEMPTION.**

Este directorio contiene el **prototipo del kernel de OMEGA**.
No es un kernel completo, pero es la base sobre la que se construirá.

## Estado actual

El kernel prototype **funciona** y soporta:

- ✅ Boot bare-metal aarch64 en EL1
- ✅ UART (PL011, QEMU virt)
- ✅ Vector table + excepciones (SYNC, IRQ, FIQ, SError)
- ✅ ARM Generic Timer
- ✅ GIC (Generic Interrupt Controller)
- ✅ Interrupciones reales del timer
- ✅ Multitarea cooperativa (`task_yield()`)
- ✅ Preemption con timer (cambio de contexto desde IRQ)
- ✅ IPC básico (endpoints + mensajes)
- ✅ Capabilities (object capabilities para endpoints)
- ✅ Paso de capabilities en mensajes IPC

## Qué NO hace todavía

- ❌ MMU (sin memoria virtual)
- ❌ User space (todo corre en EL1)
- ❌ Prioridades en el scheduler
- ❌ SMP (un solo CPU)
- ❌ Drivers más allá del UART, GIC y timer
- ❌ Sistema de archivos
- ❌ Red

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
│       ├── exceptions.h
│       ├── task.h
│       ├── ipc.h
│       └── cap.h
├── arch/
│   └── arm64/
│       ├── README.md
│       ├── boot.S            (entry point)
│       ├── exceptions_asm.S  (vector table + stub IRQ)
│       ├── switch.S          (cambio de contexto)
│       ├── exceptions.c
│       ├── gic.c
│       ├── timer.c
│       ├── uart.c
│       ├── task.c
│       ├── ipc.c
│       └── cap.c
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

Salida esperada (con preemption):

```
OMEGA kernel v11 (preemption)
---
Inicializando excepciones...
Vector table instalada.
Inicializando GIC...
[gic] GICD_TYPER = 0x0000000000000008
[gic] GICD_CTLR = 0x0000000000000001
[gic] GICC_PMR = 0x00000000000000ff
[gic] GICC_CTLR = 0x0000000000000001
Habilitando IRQ del timer (PPI 30)...
[gic] ISENABLER[0] = 0x000000004000ffff
Programando timer para 100 ms...
Timer programado.
Inicializando IPC...
Inicializando capabilities...
Inicializando tareas...
Tareas creadas: A=0, B=1
Habilitando IRQs en el CPU...
IRQs habilitadas.
---
Iniciando scheduler (preemptivo)...
[A][A][A]...[B][B][B]...[A][A][A]...
```

Las tareas A y B se alternan automáticamente (preemption con timer
a 100 ms).

Salir de QEMU: Ctrl+A luego X.

Reglas del proyecto

1. No usar FP/SIMD (-mgeneral-regs-only).
2. Alinear structs a 8/16 bytes (__attribute__((aligned(8/16)))).
3. Guardar contexto completo en handlers de excepción (x0-x30 + SP + PC + SPSR).
4. Usar eret para retornar de excepciones recuperables.
5. No afirmar que algo funciona sin probarlo.
6. Documentar decisiones arquitectónicas como ADR.

Limitaciones conocidas

· El IPC no tiene control de acceso (el modelo de capabilities está en progreso).
· El IPC no tiene blocking (send no espera si el buzón está lleno).
· Las tareas pueden ceder el control con task_yield() además de la preemption.
· El stack de cada tarea es de 4 KB.
· El número máximo de tareas es 8.
· El kernel corre íntegramente en EL1.
· La UART no está sincronizada: puede haber basura ocasional en la salida.

Roadmap del kernel

1. ✅ Boot en QEMU
2. ✅ UART
3. ✅ Vector table + excepciones
4. ✅ ARM Generic Timer (polling)
5. ✅ GIC + IRQ real
6. ✅ Multitarea cooperativa
7. ✅ IPC básico
8. ✅ Preemption con timer
9. ⏳ MMU + memoria virtual
10. ⏳ Capabilities en IPC (control de acceso real)
11. ⏳ User space
12. ⏳ Drivers
13. ⏳ Sistema de archivos
14. ⏳ Red

Ver también

· docs/decisions/ADR-0002-kernel-prototype.md — Decisiones de arquitectura.
· docs/decisions/ADR-0003-scheduler.md — Modelo de scheduler.
· docs/decisions/ADR-0004-preemption-timer.md — Preemption con timer.
· docs/development/README.md — Reglas del proyecto.EOF

