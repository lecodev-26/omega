
## Regla sobre Makefiles y tabs

Los Makefiles requieren tabs reales al inicio de las líneas de receta.
Los heredocs (`cat > archivo <<'EOF'`) **no preservan tabs de forma fiable**
al copiar/pegar en Termux.

**Método adoptado:**

1. Cabecera (variables) con `cat > Makefile <<'EOF'`.
2. Reglas (con tabs) con `printf '\t...' >> Makefile` línea por línea.
3. Verificar con `grep -Pc '^\t' Makefile` (debe ser > 0).
4. Verificar con `cat -A Makefile` (las tabs aparecen como `^I`).

Esta regla se aplica a cualquier archivo con indentación significativa:
Makefiles, Python, YAML.

## Regla sobre printf y strings que empiezan por guion

En bash, `printf '- texto'` falla con "invalid option" porque interpreta
`-` como una opción. La solución es usar el separador `--`:

```bash
# INCORRECTO
printf -- '- item\n'  # Fallará

# CORRECTO
printf -- '- item\n'  # OK
```

Esta regla se aplica siempre que el string a imprimir empiece por -.

## Regla sobre linker bare-metal en Termux

`clang` en Termux tiene problemas invocando el linker para targets
bare-metal (`aarch64-unknown-none`). El error es que intenta invocar
`/data/data/com.termux/files/usr/bin/false` en lugar del linker real.

**Solución adoptada:** invocar `ld.lld` directamente en el Makefile,
sin `clang` como driver del linker.

Ejemplo:

```makefile
CC := clang
LD := ld.lld

TARGET  := aarch64-unknown-none
CFLAGS  := --target=$(TARGET) -ffreestanding -fno-stack-protector
LDFLAGS := -T linker.ld

$(BIN): $(OBJS)
$(LD) $(LDFLAGS) $^ -o $@
```

Nota: ld.lld no acepta --target. El target lo determina a partir
de los objetos de entrada.

Verificado: compila y bootea en QEMU virt aarch64.

## Regla sobre FP/SIMD en bare-metal aarch64

En bare-metal aarch64, el acceso a registros FP/SIMD (registros `v0`-`v31`,
`s0`-`s31`, `d0`-`d31`, `q0`-`q31`) está deshabilitado por defecto en
`CPACR_EL1.FPEN`. Si el compilador genera instrucciones SIMD (por ejemplo,
`movi v0.2d, #0`), se produce una excepción no manejable que cuelga el
sistema.

**Solución:** compilar con `-mgeneral-regs-only`, que instruye al compilador
para no usar FP/SIMD.

**Verificado:** el cuelgue de `exceptions_init` se debía a una instrucción
`movi v0.2d, #0` generada por clang al agrupar inicializaciones.

## Regla sobre división por cero en ARMv8-A

En ARMv8-A, las instrucciones `sdiv` y `udiv` **NO generan excepción** cuando
el divisor es 0. Devuelven 0 como resultado (según ARM ARM). Esto es diferente
de x86, donde `div` con divisor 0 genera `#DE`.

Para provocar una excepción síncrona de prueba, usar `brk #N` (breakpoint),
`udf #N` (undefined instruction), o acceder a memoria no mapeada.

## Regla sobre contexto de registros en handlers

Todo handler de excepción o interrupción **debe guardar y restaurar el
contexto completo de registros** (x0-x30 en aarch64) antes de retornar.

**Verificado:** un handler que guardaba solo x1-x4 y LR corrompía los
registros x0, x5-x28 del código interrumpido, causando cuelgues
inexplicables.

**Estructura mínima del stub:**
- Reservar espacio en el stack (272 bytes = 17 × 16).
- Guardar x0-x30 con `stp` en pares.
- Leer system registers (ESR_EL1, ELR_EL1, FAR_EL1, SPSR_EL1).
- Llamar a la función C del handler.
- Restaurar x0-x30.
- Retornar con `eret`.

**Nota sobre `eret`:** después de `eret`, la CPU vuelve a `ELR_EL1` con
el estado en `SPSR_EL1`. Para que esto funcione correctamente, los
system registers deben haberse preservado (no modificados por el handler).

**Handlers no recuperables:** si la excepción no es recuperable (por
ejemplo, un `brk` inesperado), el handler puede decidir no retornar y
quedarse en bucle infinito. Esto es válido pero debe ser una decisión
consciente.

## Regla sobre multitarea cooperativa

El cambio de contexto cooperativo entre tareas requiere:

1. **Guardar solo callee-saved** (x19-x28, x29/FP, x30/LR, SP, PC) en la ABI AAPCS64.
2. **Inicializar el stack de una tarea nueva** apuntando al tope alineado a 16 bytes.
3. **PC de una tarea nueva** apunta a un trampoline que llama a su entry point.
4. **Trampoline por tarea**: `task_trampoline` recibe el puntero a la task en x19, extrae el entry, y lo llama. Si retorna, marca la task como `FINISHED`.
5. **El primer `task_yield()`** no tiene tarea previa. Usa `task_start_first` que solo carga el contexto de la siguiente tarea y salta.

**Estructura del `task_context_t`:**
- x19-x28 (10 registros)
- x29 (FP)
- x30 (LR)
- SP
- PC

Total: 14 × 8 = 112 bytes.

**Verificado:** 2 tareas cooperativas (`[A][B][A][B]...`) funcionan correctamente en QEMU virt aarch64.
