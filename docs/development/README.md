
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

El `task_context_t` contiene x0-x30 + SP + PC + SPSR. Este es el contexto
necesario para reanudar una tarea interrumpida por IRQ.

**Layout (offsets en bytes):**
- 0..240: x[0]..x[30] (31 registros de 8 bytes)
- 248: sp
- 256: pc (ELR_EL1)
- 264: spsr

**Uso cooperativo:** Solo se guardan/restauran los callee-saved
(x19-x30) + SP + PC. El resto no se toca.

**Uso preemptivo (ACTIVADO):** El stub de excepción guarda x0-x30 completo
en el stack de la tarea. El cambio de contexto desde IRQ
(`context_switch_from_irq`) lee/escribe el contexto extendido completo.

**Verificado:** el kernel prototype compila y funciona con multitarea
preemptiva en QEMU virt aarch64.

## Regla sobre preemption con timer

La preemption está ACTIVADA. El timer del sistema interrumpe cada 100 ms
y fuerza el cambio de tarea.

**Componentes:**

- `timer_irq_handler()` rearma el timer.
- `task_tick_from_irq()` decide la siguiente tarea. Actualiza
  `g_preempt_from_idx` y `g_preempt_next_idx`.
- El stub de IRQ, al retornar de C, verifica `g_preempt_next_idx`:
  - Si `< 0`: retorno normal (restaurar x0-x30 del stack + `eret`).
  - Si `>= 0`: llamar a `context_switch_from_irq(from, to)`.
- `context_switch_from_irq` hace el cambio completo y `eret`.

**Detalles críticos:**

1. **`SPSR` de tareas nuevas debe ser `0x305`**, no `0x3C5`.
   - `0x305` = `EL1h` + `D=1, A=1, I=0, F=0` (IRQs habilitadas).
   - `0x3C5` = `EL1h` + `D=1, A=1, I=1, F=1` (IRQs enmascaradas).
   - Si es `0x3C5`, la tarea arranca con IRQs deshabilitadas y el
     timer nunca interrumpe.

2. **El stack de tareas nuevas debe prepararse en `task_create`**
   como si hubieran sido interrumpidas: escribir x0-x30 en el marco de
   256 bytes, con `x19 = puntero a task`. Sin esto,
   `context_switch_from_irq` carga basura en `x19` al cambiar a una
   tarea nueva.

3. **`task_tick_from_irq` solo debe llamarse UNA vez por tick.** Llamarla
   dos veces hace que el scheduler cambie a B y vuelva a A inmediatamente.

4. **`eret` no restaura SP.** Usa `SP_EL1` directamente. Por eso
   `context_switch_from_irq` ajusta `sp` al SP original de la tarea
   antes de `eret`.

5. **Al cargar x0-x30 del stack a los registros, x2 y x3 se cargan al
   final.** Porque x2, x3 se usan como temporales durante el proceso.

**Ver:** `docs/decisions/ADR-0004-preemption-timer.md`.

## Regla sobre IPC blocking

`ipc_send` y `ipc_recv` bloquean si el buzón está lleno/vacío. La tarea
se pone en estado BLOCKED y el scheduler elige otra.

**Componentes:**

- `TASK_STATE_BLOCKED` — estado de tarea bloqueada.
- `task_block_current()` — bloquea la tarea actual y cede el control.
- `task_unblock(idx)` — desbloquea la tarea `idx` (pone READY).
- `ipc_send` — si el buzón está lleno, bloquea hasta que haya hueco.
- `ipc_recv` — si el buzón está vacío, bloquea hasta que llegue mensaje.

**Detalles críticos:**

1. **Secciones críticas con IRQs deshabilitadas.** `ipc_send`/`ipc_recv`
   acceden a las colas dentro de secciones críticas. Esto evita que el
   timer interrumpa entre `ep->count++` y `task_unblock()`, lo cual
   causaba deadlock.

2. **`task_yield` es atómico.** Deshabilita IRQs alrededor del cambio
   de contexto cooperativo. Evita que el timer interrumpa en medio.

3. **Excepción en `task_yield` para el arranque inicial.** En `prev < 0`
   (primera tarea), no se deshabilitan IRQs, porque la tarea nueva debe
   arrancar con IRQs habilitadas.

**Ver:** `docs/decisions/ADR-0005-ipc-blocking.md`.

## Regla sobre `daif` save/restore

En lugar de spinlocks, el kernel usa `daif` save/restore para
sincronización. En un sistema uniprocesador, deshabilitar IRQs es
suficiente.

**Sintaxis:**

```c
uint64_t daif = uart_lock();   /* guarda DAIF, deshabilita IRQs */
/* ... sección crítica ... */
uart_unlock(daif);             /* restaura DAIF */
```

Uso actual:

· UART: uart_puts, uart_puthex64, uart_putdec32.
· IPC: ipc_send, ipc_recv, ipc_send_with_cap.
· Scheduler: task_yield (alrededor de context_switch).

Regla: nunca mantener el lock mientras se llama a task_block_current
o a cualquier función que pueda bloquear. Deshabilitar IRQs, hacer la
operación crítica, rehabilitar IRQs, y luego bloquear si es necesario.

Verificado: resuelve deadlocks por reentrada y carreras con el timer.

Regla sobre task_yield atómico

task_yield no es reentrante y debe ser atómico respecto al timer.

Problema: si el timer interrumpe a A justo después de que A ponga
g_current = B pero antes de context_switch, el handler de IRQ ve
g_current = B, y puede causar corrupción de contexto o deadlock.

Solución: deshabilitar IRQs alrededor del cambio de contexto
cooperativo.

```c
uint64_t daif;
__asm__ volatile("mrs %0, daif" : "=r"(daif));
__asm__ volatile("msr daifset, #2" ::: "memory");

g_current = next;
context_switch(&g_tasks[prev].context, &g_tasks[next].context);

__asm__ volatile("msr daif, %0" :: "r"(daif) : "memory");
```

Excepción: en el arranque inicial (prev < 0), no se deshabilitan
IRQs, porque la tarea nueva debe arrancar con IRQs habilitadas.

Ver: docs/decisions/ADR-0005-ipc-blocking.md.

Regla sobre g_current

g_current es el índice de la tarea actualmente en ejecución. Debe
actualizarse antes de cualquier cambio de contexto.

Errores conocidos:

1. En task_yield, cuando prev < 0 (arranque inicial), g_current
   no se actualizaba. Resultado: task_current() devolvía NULL, y
   cap_lookup fallaba, rompiendo el IPC.
2. En task_yield, en el cambio cooperativo, g_current no se
   actualizaba. Resultado: task_current() devolvía la tarea equivocada
   cuando B arrancaba.

Solución: actualizar g_current = next antes de task_start_first
y antes de context_switch.

Regla sobre el orden de las operaciones en IPC

En ipc_send/ipc_recv, el orden correcto es:

1. Comprobar si hay hueco/mensaje (con IRQs off).
2. Si hay, escribir/leer y desbloquear (con IRQs off).
3. Si no hay, rehabilitar IRQs y bloquear la tarea.

Nunca bloquear la tarea con IRQs deshabilitadas. Si se hace, el
timer no puede despertarla y hay deadlock.

Ejemplo correcto:

```c
while (1) {
    uint64_t daif = uart_lock();

    if (ep->count < IPC_QUEUE_SIZE) {
        ep->messages[ep->tail] = *msg;
        ep->tail = (ep->tail + 1) % IPC_QUEUE_SIZE;
        ep->count++;

        ipc_wake_one_blocked();
        uart_unlock(daif);
        return 0;
    }

    uart_unlock(daif);
    task_block_current();
}
```

Ejemplo incorrecto (deadlock):

```c
uint64_t daif = uart_lock();
while (ep->count >= IPC_QUEUE_SIZE) {
    task_block_current();   /* ← IRQs deshabilitadas, timer no puede despertar */
}
/* ... */
uart_unlock(daif);
```

