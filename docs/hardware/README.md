
## Estrategia de compilación

| Target | Arquitectura | Sistema | Estado |
|--------|--------------|---------|--------|
| `aarch64-unknown-linux-android` | aarch64 | Android/Termux | ✅ Nativo |
| `aarch64-unknown-linux-gnu` | aarch64 | Linux | ⏳ Pendiente |
| `aarch64-unknown-none` | aarch64 | Bare-metal | ⏳ Pendiente |
| `aarch64-freestanding` | aarch64 | Bare-metal | ⏳ Pendiente |

### Termux como entorno de desarrollo

Termux corre en `aarch64-unknown-linux-android24` y compila nativamente
para esta arquitectura. Esto permite:

- Compilar el host prototype sin cross-compile.
- Verificar que el código funciona en ARM64.
- Preparar para bare-metal y para la EVK.

### Cross-compile (futuro)

Cuando necesitemos compilar para bare-metal (kernel OMEGA) o para
targets específicos de la IMX8MP-EVK, necesitaremos:

- Toolchain bare-metal `aarch64-none-elf` o equivalente.
- Device tree específico de i.MX8M Plus.
- Bootloader/U-Boot configurado para la placa.

Esto queda **pendiente** hasta que tengamos la EVK.
