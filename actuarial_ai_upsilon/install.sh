#!/bin/bash

# Script de instalación para ActuarialAI Native App
# Requiere: SDK de Upsilon, herramientas ARM, NumWorks con firmware Upsilon

set -e

echo "=== ActuarialAI Native App Installer ==="
echo ""

# Verificar herramientas necesarias
echo "Verificando herramientas..."

if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "Error: arm-none-eabi-gcc no encontrado"
    echo "Instalar con: sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "Error: make no encontrado"
    echo "Instalar con: sudo apt-get install build-essential"
    exit 1
fi

echo "✓ Herramientas ARM encontradas"

# Verificar SDK de Upsilon
UPSILON_DIR="/tmp/Upsilon"
if [ ! -d "$UPSILON_DIR" ]; then
    echo "Clonando SDK de Upsilon..."
    cd /tmp
    git clone --recurse-submodules https://github.com/UpsilonNumworks/Upsilon.git
    echo "✓ SDK de Upsilon clonado"
else
    echo "✓ SDK de Upsilon encontrado"
fi

# Copiar aplicación
echo "Copiando aplicación ActuarialAI..."
DEST_DIR="$UPSILON_DIR/apps/external/actuarial_ai"

if [ -d "$DEST_DIR" ]; then
    echo "Eliminando versión anterior..."
    rm -rf "$DEST_DIR"
fi

mkdir -p "$DEST_DIR"
cp -r * "$DEST_DIR/"
echo "✓ Aplicación copiada"

# Configurar compilación
echo "Configurando compilación..."
cd "$UPSILON_DIR"

# Usar la versión completa por defecto
cp "$DEST_DIR/sources_complete.mak" "$DEST_DIR/sources.mak"

echo "✓ Configuración lista"

# Compilar
echo "Compilando aplicación nativa..."
echo "Esto puede tomar varios minutos..."

EXTAPP_PATH=apps/external/actuarial_ai/ make PLATFORM=device clean
EXTAPP_PATH=apps/external/actuarial_ai/ make PLATFORM=device

if [ $? -eq 0 ]; then
    echo "✓ Compilación exitosa"
else
    echo "✗ Error en compilación"
    exit 1
fi

# Verificar calculadora conectada
echo ""
echo "Verificando conexión con NumWorks..."

if lsusb | grep -q "NumWorks"; then
    echo "✓ NumWorks detectada"
    
    echo ""
    echo "¿Instalar en la calculadora? (y/n)"
    read -r response
    
    if [[ "$response" =~ ^[Yy]$ ]]; then
        echo "Instalando en NumWorks..."
        make actuarial_ai_flash
        
        if [ $? -eq 0 ]; then
            echo "✓ Instalación exitosa"
            echo ""
            echo "La aplicación ActuarialAI está ahora instalada en tu NumWorks."
            echo "Busca 'ActuarialAI' en el menú de aplicaciones."
        else
            echo "✗ Error en instalación"
            exit 1
        fi
    else
        echo "Instalación cancelada por el usuario."
    fi
else
    echo "⚠ NumWorks no detectada"
    echo "Conecta tu calculadora via USB y ejecuta:"
    echo "cd $UPSILON_DIR && make actuarial_ai_flash"
fi

echo ""
echo "=== Instalación completada ==="
echo ""
echo "Archivos creados:"
echo "- Aplicación nativa compilada"
echo "- Implementación UART real"
echo "- Interfaz de usuario completa"
echo ""
echo "Próximos pasos:"
echo "1. Conectar hardware UART (NumWorks ↔ Raspberry Pi)"
echo "2. Configurar Raspberry Pi con scripts de bridge"
echo "3. Configurar Google Cloud Function"
echo "4. ¡Probar el sistema completo!"
echo ""
echo "Para más información, ver README.md" 