# OMEGA — Kernel (prototipo)

**ESTADO: PROTOTIPO FUNCIONAL.**

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
- ✅ IPC básico (endpoints + mensajes)
- ✅ Capabilities (object capabilities para endpoints)
- ✅ Paso de capabilities en mensajes IPC

## Qué NO hace todavía

- ❌ Preemption (las tareas ceden el control explícitamente)
- ❌ MMU (sin memoria virtual)
- ❌ Capabilities (el IPC usa números de endpoint)
- ❌ User space (todo corre en EL1)
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
│       └── ipc.h
├── arch/
│   └── arm64/
│       ├── README.md
│       ├── boot.S            (entry point)
│       ├── exceptions_asm.S  (vector table)
│       ├── switch.S          (cambio de contexto)
│       ├── exceptions.c
│       ├── gic.c
│       ├── timer.c
│       ├── uart.c
│       ├── task.c
│       └── ipc.c
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
OMEGA kernel v7 (IPC)
---
Inicializando excepciones...
Vector table instalada.
Inicializando IPC...
Inicializando tareas...
Tareas creadas: A=0, B=1
---
Iniciando scheduler...
[A->B ping][B: pong enviado][A<-B pong]...
```

Reglas del proyecto

1. No usar FP/SIMD (-mgeneral-regs-only).
2. Alinear structs a 8/16 bytes (__attribute__((aligned(8/16)))).
3. Guardar contexto completo en handlers de excepción.
4. Usar eret para retornar de excepciones recuperables.
5. No afirmar que algo funciona sin probarlo.

Limitaciones conocidas

· El IPC no tiene control de acceso (cualquier tarea puede enviar a cualquier endpoint).
· El IPC no tiene blocking (send no espera si el buzón está lleno).
· Las tareas deben ceder el control explícitamente.
· El stack de cada tarea es de 4 KB.
· El número máximo de tareas es 8.
· El kernel corre íntegramente en EL1.

Roadmap del kernel

1. ✅ Boot en QEMU
2. ✅ UART
3. ✅ Vector table + excepciones
4. ✅ ARM Generic Timer (polling)
5. ✅ GIC + IRQ real
6. ✅ Multitarea cooperativa
7. ✅ IPC básico
8. ⏳ Preemption con timer
9. ⏳ MMU + memoria virtual
10. ⏳ Capabilities
11. ⏳ User space
12. ⏳ Drivers
13. ⏳ Sistema de archivos
14. ⏳ Red

Ver también

· docs/decisions/ADR-0002-kernel-prototype.md — Decisiones de arquitectura.
· docs/development/README.md — Reglas del proyecto.
  EOF
