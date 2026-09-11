
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
