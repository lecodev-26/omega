# OMEGA — Host Prototype

**Estado:** Prototipo conceptual v3.

**NO ES el kernel de OMEGA.** Es un banco de pruebas que se ejecuta en
Termux (o en cualquier sistema POSIX con `clang` y `make`).

## Qué modela

- **Mensajes** de IPC con payload de tamaño fijo (8, 64, 512, 4096 bytes).
- **Endpoints** como colas en memoria.
- **Capabilities** con derechos, derivación, delegación y revocación.
- **Servicios** con nombre, inbox y handler (echo, time).

## Qué NO modela

- No hay kernel.
- No hay scheduling real.
- No hay memoria virtual.
- No hay aislamiento real entre procesos.
- No hay IPC entre procesos reales.
- No hay concurrencia real.

## Compilar y ejecutar

```bash
cd platforms/host
make CC=clang
make run
```

Tests

```bash
cd platforms/host
make test
```

Estructura

```
platforms/host/
├── include/omega/
│   ├── message.h
│   ├── endpoint.h
│   ├── capability.h
│   └── service.h
├── src/
│   ├── message.c
│   ├── endpoint.c
│   ├── capability.c
│   ├── service.c
│   └── main.c
├── tests/
│   ├── test_message.c
│   ├── test_endpoint.c
│   ├── test_capability.c
│   └── test_service.c
└── Makefile
```

Limitaciones conocidas

· El modelo de servicios es secuencial: no hay scheduling ni
  concurrencia. Un servicio se procesa explícitamente con
  omega_service_step.
· No hay endpoint de retorno. El caller recibe la respuesta como
  out-parameter de omega_service_step.
· La delegación de capabilities es una operación en memoria, sin
  kernel que la valide.

Siguiente paso

El siguiente paso es preparar la base para el experimento del Piloto 1
sobre esta infraestructura, primero como simulación host y después
sobre la IMX8MP-EVK.

Regla

Todo lo que este prototipo demuestra es conceptual. No sustituye a la
verificación en hardware ni a la experimentación.
