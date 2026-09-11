# Changelog

## [Unreleased]

### Added
- Repositorio inicial con documentación estructurada.
- Host prototype v3: mensajes, endpoints, capabilities, servicios.
- Simulación host del Piloto 1 IPC con análisis estadístico.
- Kernel prototype:
  - Boot bare-metal aarch64 en QEMU virt.
  - UART PL011.
  - Vector table + excepciones (SYNC, IRQ, FIQ, SError).
  - ARM Generic Timer.
  - GIC + IRQs reales.
  - Multitarea cooperativa.
  - IPC básico (endpoints + mensajes).
- Documentación:
  - 19 documentos conceptuales (0.1.0–0.1.18).
  - Requisitos y propiedades verificables (R1-FINAL).
  - Diseño experimental (R2).
  - Protocolo Piloto 1.
  - ADR-0001: Fundación del kernel.
  - ADR-0002: Kernel prototype.
  - Documento de brecha conceptual.

### Notes
- No hay hardware real disponible todavía.
- El kernel prototype corre únicamente en QEMU.
- No hay capabilities, MMU ni user space todavía.
