# Mapa Conceptual de OMEGA

Este documento mapea qué áreas de conocimiento han sido investigadas
y dónde está la documentación correspondiente.

## Fases de investigación

| Área | Documentos | Estado |
|------|-----------|--------|
| Visión | 0.1.0 | Congelado |
| Problema | 0.1.1 | Congelado |
| Arquitectura fundamental | 0.1.2 | Congelado |
| Capabilities | 0.1.3 | Congelado |
| IPC | 0.1.4 | Congelado |
| Memoria | 0.1.5 | Congelado |
| Procesos/scheduling | 0.1.6 | Congelado |
| Arranque | 0.1.7 | Congelado |
| Drivers/IRQ | 0.1.8 | Congelado |
| Servicios | 0.1.9 | Congelado |
| Compatibilidad | 0.1.10 | Congelado |
| Actualizaciones | 0.1.11 | Congelado |
| Seguridad | 0.1.12 | Congelado |
| Desarrollo/herramientas | 0.1.13 | Congelado |
| Comunidad/gobernanza | 0.1.14 | Congelado |
| Documentación/comunicación | 0.1.15 | Congelado |
| Financiación | 0.1.16 | Congelado |
| Testing | 0.1.17 | Congelado |
| Observabilidad | 0.1.18 | Congelado |

## Fases de síntesis

| Área | Documento | Estado |
|------|-----------|--------|
| Requisitos | R1-FINAL | Congelado |
| Diseño experimental | R2 | Congelado |
| Protocolo Piloto 1 | IPC | Cerrado como propuesta |

## Principio rector

> Conocer → comparar → experimentar → decidir → construir.

## ADRs

| ADR | Título | Estado |
|-----|--------|--------|
| ADR-0001 | Fundación del kernel | Reemplazado |
| ADR-0002 | Kernel prototype | Aceptado (parcialmente superado) |
| ADR-0003 | Scheduler | Aceptado |
| ADR-0004 | Preemption con timer | Aceptado |
| ADR-0005 | IPC blocking | Aceptado |

## Kernel prototype — estado por componente

| Componente | Estado | Verificado |
|-----------|--------|-----------|
| Boot aarch64 | ✅ | QEMU virt |
| UART PL011 | ✅ | QEMU virt |
| Excepciones | ✅ | QEMU virt |
| ARM Generic Timer | ✅ | QEMU virt |
| GICv2 + IRQs reales | ✅ | QEMU virt |
| Multitarea cooperativa | ✅ | QEMU virt |
| Multitarea preemptiva | ✅ | QEMU virt |
| IPC básico | ✅ | QEMU virt |
| IPC blocking | ✅ | QEMU virt |
| Capabilities | ✅ | QEMU virt |
| Paso de caps en IPC | ✅ | QEMU virt |
| MMU | ❌ | No iniciado |
| User space (EL0) | ❌ | No iniciado |
| Drivers (más) | ❌ | No iniciado |
| Sistema de archivos | ❌ | No iniciado |
| Red | ❌ | No iniciado |
