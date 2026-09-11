
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
