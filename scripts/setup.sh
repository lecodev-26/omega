#!/data/data/com.termux/files/usr/bin/bash
# OMEGA — Setup script
# Verifica dependencias básicas y prepara el entorno.

set -e

echo "=== OMEGA setup ==="
echo ""

echo "[1/4] Verificando Git..."
if ! command -v git >/dev/null 2>&1; then
    echo "ERROR: git no está instalado."
    echo "Instalar con: pkg install git"
    exit 1
fi
git --version

echo ""
echo "[2/4] Verificando herramientas básicas..."
for tool in curl wget make; do
    if command -v "$tool" >/dev/null 2>&1; then
        echo "OK: $tool"
    else
        echo "FALTA: $tool (instalar con: pkg install $tool)"
    fi
done

echo ""
echo "[3/4] Verificando compilador..."
if command -v clang >/dev/null 2>&1; then
    clang --version | head -1
elif command -v gcc >/dev/null 2>&1; then
    gcc --version | head -1
else
    echo "FALTA: compilador (instalar con: pkg install clang)"
fi

echo ""
echo "[4/4] Verificando Python..."
if command -v python >/dev/null 2>&1; then
    python --version
elif command -v python3 >/dev/null 2>&1; then
    python3 --version
else
    echo "FALTA: python (instalar con: pkg install python)"
fi

echo ""
echo "=== Setup completado ==="
echo ""
echo "Siguiente paso: ./scripts/build.sh (cuando exista código)"
