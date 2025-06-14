# 🚀 Comandos para Subir ActuarialAI a GitHub

## 📋 Pasos Exactos

### 1. Crear repositorio en GitHub
- Ve a: https://github.com/new
- Nombre: `actuarial-ai-upsilon`
- Descripción: `🧮 ActuarialAI - Professional Actuarial Calculator for NumWorks with Upsilon`
- Público ✅
- Click "Create repository"

### 2. Ejecutar estos comandos en tu terminal:

```bash
# Configurar repositorio remoto (reemplaza TU_USUARIO con tu usuario de GitHub)
git remote add origin https://github.com/TU_USUARIO/actuarial-ai-upsilon.git

# Cambiar a rama main
git branch -M main

# Subir código
git push -u origin main
```

### 3. Activar GitHub Actions
- Ve a tu repositorio: `https://github.com/TU_USUARIO/actuarial-ai-upsilon`
- Click en pestaña "Actions"
- Si aparece un botón "I understand my workflows, go ahead and enable them", haz click

### 4. Ejecutar compilación
- En la pestaña "Actions"
- Click en "Build Upsilon with ActuarialAI" (lado izquierdo)
- Click en "Run workflow" (botón verde)
- Click en "Run workflow" en el popup
- ⏳ Espera 10-15 minutos

### 5. Descargar resultado
- Cuando termine (✅ verde), click en el workflow completado
- Scroll hacia abajo hasta "Artifacts"
- Descarga: `ActuarialAI-Upsilon-Complete-Package`

### 6. Usar tu simulador
- Extrae el archivo ZIP descargado
- Abre: `web-simulator/epsilon.html` en tu navegador
- Navega a: Menú → External → ActuarialAI
- 🎉 ¡Disfruta tu aplicación actuarial!

## 🎯 Resultado Final

Tendrás:
- ✅ **Simulador Web** - Funciona en cualquier navegador
- ✅ **Simulador macOS** - Aplicación nativa para Mac
- ✅ **Aplicación ActuarialAI** - Integrada en Upsilon
- ✅ **Código fuente** - Disponible en GitHub
- ✅ **Compilación automática** - Cada vez que hagas cambios

## 🔧 Para Instalar en NumWorks Real

1. Descarga firmware Upsilon desde: https://upsilonnumworks.github.io/Upsilon-website/
2. Flashea tu NumWorks con Upsilon
3. ActuarialAI aparecerá automáticamente en External apps

## 💡 Ventajas de este Método

- ✅ **Sin dependencias locales** - Todo se compila en GitHub
- ✅ **Multiplataforma** - Web + macOS + NumWorks real
- ✅ **Automático** - Push código → Compilación automática
- ✅ **Profesional** - Mismo proceso que proyectos open source grandes
- ✅ **Compartible** - Otros pueden usar tu aplicación fácilmente 