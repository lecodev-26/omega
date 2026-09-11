# OMEGA — Host Prototype

**Estado:** Prototipo conceptual.

**NO ES el kernel de OMEGA.** Es un banco de pruebas que se ejecuta en
Termux (o en cualquier sistema POSIX con `clang` y `make`).

## Objetivo

Validar los conceptos de:

- Mensajes de IPC con payload de tamaño fijo.
- Endpoints como colas en memoria.
- Capabilities con derechos, derivación y revocación.

## Lo que NO hace

- No hay kernel.
- No hay scheduling real.
- No hay memoria virtual.
- No hay aislamiento real entre procesos.
- No hay IPC entre procesos reales.

## Compilar y ejecutar

```bash
cd platforms/host
make
make run

test
cd platforms/host
make test

estructura

platforms/host/
├── include/omega/
│   ├── message.h
│   ├── endpoint.h
│   └── capability.h
├── src/
│   ├── message.c
│   ├── endpoint.c
│   ├── capability.c
│   └── main.c
├── tests/
│   ├── test_message.c
│   ├── test_endpoint.c
│   └── test_capability.c
└── Makefile
Siguiente paso

Una vez validado este prototipo, el siguiente paso es trasladar estos
conceptos a ARM64 (build cross-compile) y posteriormente a hardware real.

Regla

Todo lo que este prototipo demuestra es conceptual. No sustituye a la
verificación en hardware ni a la experimentación.
