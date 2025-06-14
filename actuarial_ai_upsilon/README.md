# ActuarialAI - Aplicación Nativa para NumWorks

Una aplicación nativa para calculadoras NumWorks que proporciona cálculos actuariales avanzados usando inteligencia artificial.

## 🎯 Características

- **Interfaz nativa**: Aplicación compilada en C que se ejecuta directamente en el hardware NumWorks
- **Comunicación UART**: Conecta con Raspberry Pi para procesamiento en la nube
- **IA Actuarial**: Utiliza Google Cloud y Gemini AI para cálculos complejos
- **Menú interactivo**: Navegación intuitiva con las teclas de la calculadora
- **Múltiples cálculos**: Seguros de vida, anualidades, tablas de mortalidad, interés compuesto

## 🏗️ Arquitectura del Sistema

```
NumWorks Calculator (Aplicación Nativa)
           ↓ UART (115200 baud)
    Raspberry Pi Zero 2W
           ↓ HTTP/WiFi
      Google Cloud Function
           ↓ API
        Gemini AI
```

## 📋 Requisitos

### Hardware
- Calculadora NumWorks (N0110, N0115, o N0120)
- Raspberry Pi Zero 2W
- Conexiones UART:
  - NumWorks PA11 (TX) → Pi GPIO 15 (RX)
  - NumWorks PA12 (RX) → Pi GPIO 14 (TX)
  - GND común

### Software
- Firmware Upsilon en NumWorks
- SDK de Upsilon para compilación
- Raspberry Pi OS en el Pi
- Cuenta de Google Cloud

## 🛠️ Instalación

### 1. Preparar entorno de desarrollo

```bash
# Clonar Upsilon
git clone --recurse-submodules https://github.com/UpsilonNumworks/Upsilon.git

# Copiar aplicación
cp -r actuarial_ai_upsilon Upsilon/apps/external/actuarial_ai

# Instalar dependencias (Debian/Ubuntu)
sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi build-essential
```

### 2. Compilar aplicación

```bash
cd Upsilon
EXTAPP_PATH=apps/external/actuarial_ai/ make PLATFORM=device
```

### 3. Instalar en NumWorks

```bash
# Conectar calculadora via USB
make actuarial_ai_flash
```

## 🎮 Uso de la Aplicación

### Menú Principal
1. **Life Insurance Premium** - Cálculo de primas de seguros de vida
2. **Annuity Present Value** - Valor presente de anualidades
3. **Mortality Rate Lookup** - Consulta de tablas de mortalidad
4. **Interest Calculation** - Cálculos de interés compuesto

### Controles
- **Flechas**: Navegar menú
- **OK/EXE**: Seleccionar opción
- **Back**: Regresar/Salir
- **Home**: Salir de la aplicación

### Flujo de Trabajo
1. Seleccionar tipo de cálculo
2. La aplicación envía el problema al Pi via UART
3. El Pi procesa con Google Cloud AI
4. Se muestra el resultado en pantalla

## 🔧 Configuración UART

### Protocolo de Comunicación
- **Baudrate**: 115200
- **Formato de envío**: `PROBLEM:descripción_del_problema\n`
- **Formato de respuesta**: `SOLUTION:respuesta_de_la_ia\n`
- **Timeout**: 30 segundos

### Implementación Real vs Simulada
La versión actual incluye funciones simuladas para demostración:

```c
// Función simulada - reemplazar con implementación UART real
bool send_to_pi(const char* problem) {
    // TODO: Implementar envío UART real
    return uart_ok;
}
```

## 📁 Estructura del Proyecto

```
actuarial_ai_upsilon/
├── actuarial_ai.c          # Aplicación completa con menús
├── actuarial_ai_simple.c   # Versión simplificada
├── sources.mak             # Configuración de compilación
└── README.md               # Esta documentación
```

## 🚀 Desarrollo Futuro

### Funcionalidades Pendientes
- [ ] Implementación UART real
- [ ] Múltiples tipos de seguros
- [ ] Cálculos de reservas
- [ ] Análisis de riesgo
- [ ] Guardado de resultados
- [ ] Configuración de parámetros

### Mejoras Técnicas
- [ ] Optimización de memoria
- [ ] Manejo de errores robusto
- [ ] Interfaz más avanzada
- [ ] Soporte para múltiples idiomas

## 🐛 Solución de Problemas

### Error de Compilación
```bash
# Instalar herramientas de desarrollo
sudo apt-get install build-essential gcc-arm-none-eabi

# Verificar SDK
make PLATFORM=simulator clean
```

### Problemas de UART
- Verificar conexiones de hardware
- Comprobar configuración del Pi
- Revisar permisos de puerto serie

### Aplicación no aparece
- Verificar que Upsilon esté instalado
- Confirmar que la aplicación se compiló correctamente
- Reiniciar la calculadora

## 📚 Referencias

- [Upsilon Firmware](https://github.com/UpsilonNumworks/Upsilon)
- [NumWorks Hardware](https://www.numworks.com/)
- [Raspberry Pi GPIO](https://pinout.xyz/)
- [Google Cloud Functions](https://cloud.google.com/functions)

## 📄 Licencia

Este proyecto está bajo licencia MIT. Ver archivo LICENSE para detalles.

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor:

1. Fork el proyecto
2. Crear rama para nueva funcionalidad
3. Commit los cambios
4. Push a la rama
5. Abrir Pull Request

## 📞 Soporte

Para soporte técnico:
- Crear issue en GitHub
- Consultar documentación de Upsilon
- Revisar foros de NumWorks

---

**Nota**: Esta es una aplicación de demostración. Para uso en producción, implementar las funciones UART reales y validar todos los cálculos actuariales. 