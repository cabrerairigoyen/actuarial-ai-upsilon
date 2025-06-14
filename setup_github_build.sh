#!/bin/bash

# 🚀 Configurar Compilación Automática en GitHub
# Este script configura GitHub Actions para compilar Upsilon con ActuarialAI

set -e

echo "🚀 Configurando GitHub Actions para ActuarialAI"
echo "==============================================="

# Colores
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

log() {
    echo -e "${BLUE}[$(date +'%H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✅${NC} $1"
}

warning() {
    echo -e "${YELLOW}⚠️${NC} $1"
}

# Verificar que estamos en el directorio correcto
if [ ! -f ".github/workflows/build-upsilon.yml" ]; then
    echo "❌ Error: Archivo de workflow no encontrado"
    echo "Ejecuta este script desde el directorio que contiene .github/"
    exit 1
fi

success "Archivo de workflow encontrado"

# Verificar git
if ! command -v git &> /dev/null; then
    echo "❌ Error: Git no está instalado"
    exit 1
fi

# Verificar si estamos en un repositorio git
if [ ! -d ".git" ]; then
    log "Inicializando repositorio Git..."
    git init
    success "Repositorio Git inicializado"
fi

# Configurar git si no está configurado
if [ -z "$(git config user.name)" ]; then
    log "Configurando Git..."
    echo "Ingresa tu nombre para Git:"
    read -r git_name
    git config user.name "$git_name"
    
    echo "Ingresa tu email para Git:"
    read -r git_email
    git config user.email "$git_email"
    
    success "Git configurado"
fi

# Añadir archivos al repositorio
log "Añadiendo archivos al repositorio..."
git add .github/workflows/build-upsilon.yml

# Verificar si hay cambios
if git diff --staged --quiet; then
    warning "No hay cambios nuevos para commitear"
else
    log "Commiteando cambios..."
    git commit -m "🚀 Add GitHub Actions workflow for ActuarialAI compilation

- Automated build for Upsilon web simulator
- Automated build for macOS native simulator  
- Artifact upload for easy download
- Complete release package creation
- Support for ActuarialAI external app"

    success "Cambios commiteados"
fi

echo ""
echo "🎯 PRÓXIMOS PASOS:"
echo "=================="
echo ""
echo "1. 📤 SUBIR A GITHUB:"
echo "   Si no tienes repositorio remoto:"
echo "   - Ve a https://github.com/new"
echo "   - Crea un repositorio llamado 'actuarial-ai-upsilon'"
echo "   - Ejecuta:"
echo "     git remote add origin https://github.com/TU_USUARIO/actuarial-ai-upsilon.git"
echo "     git branch -M main"
echo "     git push -u origin main"
echo ""
echo "2. 🔧 ACTIVAR GITHUB ACTIONS:"
echo "   - Ve a tu repositorio en GitHub"
echo "   - Pestaña 'Actions'"
echo "   - Habilita GitHub Actions si está deshabilitado"
echo ""
echo "3. 🚀 EJECUTAR COMPILACIÓN:"
echo "   - Ve a Actions → 'Build Upsilon with ActuarialAI'"
echo "   - Click 'Run workflow'"
echo "   - Espera ~10-15 minutos"
echo ""
echo "4. 📦 DESCARGAR RESULTADOS:"
echo "   - Cuando termine, ve a la pestaña 'Actions'"
echo "   - Click en el workflow completado"
echo "   - Descarga 'ActuarialAI-Upsilon-Complete-Package'"
echo ""
echo "5. 🧮 USAR SIMULADOR:"
echo "   - Extrae el archivo descargado"
echo "   - Abre 'web-simulator/epsilon.html'"
echo "   - ¡Disfruta ActuarialAI en Upsilon!"
echo ""
echo "💡 ALTERNATIVA RÁPIDA:"
echo "Si ya tienes el repositorio configurado:"
echo "  git push"
echo ""

# Mostrar estado actual
log "Estado actual del repositorio:"
git status --short

echo ""
success "¡Configuración completada!"
echo "Sigue los pasos arriba para compilar ActuarialAI en GitHub 🚀" 