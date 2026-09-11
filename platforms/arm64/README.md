# Platform: ARM64 / AArch64

## Estado

**Preparación conceptual.** No hay código específico todavía.

## Contexto

Termux en Android corre nativamente en `aarch64`. Esto significa que
el prototipo host ya compila para ARM64 sin necesidad de cross-compile.

## Arquitectura objetivo

- **ISA:** ARMv8-A
- **Modo:** AArch64
- **Target de desarrollo:** `aarch64-unknown-linux-android24` (Termux)
- **Target futuro:** `aarch64-unknown-none` (bare-metal, para kernel)
- **Target futuro:** `aarch64-unknown-linux-gnu` (para Linux en EVK)

## Estrategia

1. **Fase actual — nativo:** Compilar en Termux para aarch64-linux-android.
2. **Fase siguiente — bare-metal:** Preparar toolchain para `aarch64-unknown-none` (kernel OMEGA).
3. **Fase hardware — EVK:** Preparar binarios para `aarch64-linux-gnu` (Linux, QNX, seL4 en IMX8MP-EVK).

## Qué NO hace este directorio

- No contiene código de kernel.
- No contiene bootloader.
- No contiene drivers.
- No contiene bare-metal todavía.

## Regla

Todo lo que se compile aquí debe declarar explícitamente:
- arquitectura objetivo;
- sistema operativo objetivo;
- ABI objetivo.
