
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

## Regla sobre alineación de structs en bare-metal aarch64

En bare-metal aarch64, el compilador puede generar instrucciones de acceso
a memoria que **requieren alineación natural** (`stur x8`, `stp`, `ldur`,
etc.). Si el compilador asume que un struct está alineado a 8 bytes y no
lo está, el acceso provoca un **Alignment fault** (DFSC = 0x21 en el ESR).

**Regla del proyecto:**

> Toda struct en el kernel que pueda ser accedida por el compilador con
> instrucciones de 8 bytes DEBE estar declarada con la alineación
> adecuada. Usar `__attribute__((aligned(8)))` o `aligned(16)` según
> corresponda.

**Aplicación actual:**

- `task_t`: `__attribute__((aligned(16)))`
- `ipc_message_t`: `__attribute__((aligned(8)))`
- `ipc_endpoint_t`: `__attribute__((aligned(16)))`
- `task_context_t`: alineación natural (todos los campos son `uint64_t`)

**Verificado:** tres Alignment faults resueltos con esta regla:
1. `g_endpoints` en `ipc_init` (`ipc_endpoint_t` sin alinear).
2. `task_entry_point` accediendo a `t->entry` desalineado.
3. `task_a` escribiendo un `ipc_message_t` en el stack con `stur x8`.

**Cómo diagnosticar:**

1. El handler de excepciones imprime `ESR_EL1` y `FAR_EL1`.
2. Si `ESR_EL1` tiene EC = 0x25 (Data Abort) y DFSC = 0x21 (Alignment
   fault), es un problema de alineación.
3. `FAR_EL1` indica la dirección del acceso fallido.
4. `llvm-objdump -d` en `ELR_EL1` muestra la instrucción concreta.

## Regla sobre capabilities

Las capabilities en el kernel prototype son **tokens de autoridad**
que designan un objeto (por ahora, un endpoint IPC) con derechos
concretos.

**Diseño actual:**

- Cada tarea tiene una **tabla de capabilities** (CNode conceptual).
- Tamaño máximo: 16 capabilities por tarea.
- Derechos: `READ`, `WRITE`, `GRANT`, `REVOKE`.
- `ipc_send(endpoint, msg)` **requiere** una capability en la tabla
  de la tarea actual que designe `endpoint` con `WRITE`.
- Si no hay capability, `ipc_send` retorna -1 (denegado).

**Verificado:** dos tareas se comunican por IPC solo si tienen las
capabilities correspondientes. Una tarea sin capability al endpoint
destino no puede enviar.

**Limitaciones actuales:**

- No hay paso de capabilities en los mensajes.
- No hay revocación en cascada (una capability derivada no se revoca
  al revocar la madre).
- No hay capabilities de memoria, CPU ni otros objetos.
- El kernel concede capabilities manualmente en `kmain`.

**Próximos pasos:**

- Añadir paso de capabilities en mensajes IPC.
- Añadir capabilities de memoria (cuando haya MMU).
- Añadir revocación en cascada.

## Regla sobre paso de capabilities en mensajes

El paso de capabilities entre tareas en el kernel prototype sigue
el **principio de no incremento de derechos**:

1. Una tarea solo puede adjuntar una capability a un mensaje si tiene
   `CAP_RIGHT_GRANT` sobre ella.
2. La capability recibida **NO incluye** `CAP_RIGHT_GRANT` (no se propaga
   por defecto).
3. La capability recibida tiene los mismos derechos que tenía el emisor
   (menos GRANT).

**API:**

```c
ipc_send_with_cap(endpoint, msg, sender_cap_idx);
```

· sender_cap_idx es el índice en la tabla del emisor.
· Al recibir, la capability se añade automáticamente a la tabla del
  receptor (si hay espacio).
· Si no hay espacio en la tabla del receptor, el mensaje se recibe
  sin la capability (con has_cap = 0).

Verificado: una tarea A envía a B una capability al objeto 5 con
derechos READ|GRANT. B la recibe. B puede usar cap_lookup(5, READ).
B no puede usar cap_lookup(5, GRANT) (GRANT no se propagó).

Limitaciones:

· No hay paso por referencia.
· No hay "reply capabilities" (a diferencia de seL4).
· No hay revocación en cascada de las capabilities pasadas.
· Solo se puede adjuntar UNA capability por mensaje.
  EOF

## Regla sobre contexto extendido de tareas

El `task_context_t` extendido contiene x0-x30 + SP + PC + SPSR. Este es
el contexto necesario para reanudar una tarea interrumpida por IRQ.

**Layout (offsets en bytes):**
- 0..240: x[0]..x[30] (31 registros de 8 bytes)
- 248: sp
- 256: pc (ELR_EL1)
- 264: spsr

**Uso cooperativo actual:** Solo se guardan/restauran los callee-saved
(x19-x30) + SP + PC. El resto no se toca.

**Uso preemptivo (preparado, no activado):** El stub de excepción
guarda x0-x30 completo en el stack de la tarea. El cambio de contexto
desde IRQ lee/escribe el contexto extendido completo.

**Verificado:** el kernel prototype compila y funciona con el contexto
extendido, manteniendo la multitarea cooperativa.
