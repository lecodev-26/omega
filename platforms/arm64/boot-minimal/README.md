# OMEGA — Boot Minimal

**ESTADO: BANCO DE PRUEBAS.**

**ESTO NO ES EL KERNEL DE OMEGA.**

Es un binario mínimo que arranca en bare-metal sobre QEMU virt aarch64.
Su propósito es validar que la toolchain y el flujo de arranque funcionan
antes de empezar a escribir el kernel real.

## Qué hace

- Arranca desde `_start` en la dirección `0x40000000`.
- Configura el stack.
- Inicializa el UART PL011 (QEMU virt).
- Imprime un mensaje por UART.
- Lee caracteres por UART; al recibir `q`, sale limpiamente.

## Qué NO hace

- No gestiona memoria virtual.
- No gestiona interrupciones.
- No tiene scheduler.
- No tiene IPC.
- No tiene capabilities.
- No es el kernel.

## Compilar

```bash
make clean
make CC=clang
```

Inspeccionar

```bash
make inspect
```

Muestra file y readelf -h.

Ejecutar en QEMU

```bash
make run
```

Salida esperada:

```
OMEGA boot minimal
Hello from aarch64 bare-metal
---
Esto NO es el kernel de OMEGA.
Es un binario minimo de prueba.
Envia 'q' para salir.
=== fin ===
```

Escribe q para salir.

Estructura

```
boot-minimal/
├── README.md
├── linker.ld          # script del linker
├── Makefile
├── include/
│   └── uart.h
└── src/
    ├── boot.S         # punto de entrada assembly
    ├── uart.c         # driver UART PL011
    └── kmain.c        # main en C
```

Relación con el kernel real

Cuando empecemos el kernel real (kernel/), este binario seguirá
existiendo como referencia mínima de "esto es lo mínimo que debe
funcionar en bare-metal". El kernel real tendrá su propia estructura.

Regla

Todo lo que este binario demuestra es que la toolchain funciona y
que podemos arrancar en QEMU. No sustituye a verificación en hardware
ni a la experimentación.
