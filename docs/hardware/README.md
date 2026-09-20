# Hardware

Esta sección contiene la documentación de plataformas objetivo.

## Estrategia de compilación

| Target | Arquitectura | Sistema | Estado |
|--------|--------------|---------|--------|
| `aarch64-unknown-linux-android` | aarch64 | Android/Termux | ✅ Nativo |
| `aarch64-unknown-none` | aarch64 | Bare-metal | ✅ Verificado en QEMU |
| `aarch64-unknown-linux-gnu` | aarch64 | Linux | ⏳ Pendiente |
| `aarch64-freestanding` | aarch64 | Bare-metal | ⏳ Pendiente |

### Termux como entorno de desarrollo

Termux corre en `aarch64-unknown-linux-android24` y compila nativamente
para esta arquitectura. Esto permite:

- Compilar el host prototype sin cross-compile.
- Compilar el kernel bare-metal para `aarch64-unknown-none`.
- Verificar el kernel en QEMU virt aarch64.

### Kernel bare-metal

El kernel prototype se compila para `aarch64-unknown-none` y se ejecuta
en QEMU virt aarch64 (Cortex-A53). Verificado en múltiples bloques.

### Hardware real (futuro)

Cuando necesitemos compilar para hardware real (por ejemplo, IMX8MP-EVK),
necesitaremos:

- Toolchain bare-metal `aarch64-none-elf` o equivalente.
- Device tree específico del SoC.
- Bootloader/U-Boot configurado para la placa.

Esto queda **pendiente** hasta que tengamos hardware.

## Documentación relacionada

- `platforms/arm64/` — Build ARM64.
- `platforms/imx8mp/` — Preparación para IMX8MP-EVK (vacío).
- `kernel/arch/arm64/README.md` — Notas de arquitectura del kernel.
