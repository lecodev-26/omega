# Kernel Prototype — Brecha conceptual

## Propósito

Documentar la brecha entre el kernel prototype actual y la visión
arquitectónica de OMEGA (documentos 0.1.x).

## Estado del kernel prototype

El kernel prototype soporta:

- Boot bare-metal aarch64.
- UART, GIC, timer.
- Multitarea cooperativa.
- IPC básico.

## Brecha con la visión OMEGA

| Concepto (0.1.x) | Prototype | Brecha |
|------------------|-----------|--------|
| Microkernel | Minimalista pero monolítico | Mover servicios a user space |
| Capabilities | No implementado | Añadir modelo de capabilities |
| IPC con endpoints | ✅ Implementado | Añadir paso de capabilities |
| Aislamiento | Todo en EL1 | Mover tareas a EL0 + MMU |
| Servicios en user space | No implementado | Crear servicios separados |
| Multitarea | Cooperativa | Añadir preemption |
| Memoria virtual | No implementado | Añadir MMU |

## Orden propuesto para cerrar la brecha

1. **Preemption con timer** — Cerrar la brecha de multitarea.
2. **Capabilities** — Cerrar la brecha de control de acceso al IPC.
3. **MMU** — Cerrar la brecha de memoria virtual.
4. **User space (EL0)** — Cerrar la brecha de aislamiento.
5. **Servicios en user space** — Cerrar la brecha de microkernel.

## Estado

**Aceptado.**

Este documento se actualizará a medida que se cierren las brechas.
