#include <extapp_api.h>
#include <string.h>

// Colores básicos
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE  0x001F
#define GREEN 0x07E0
#define RED   0xF800

// Variables globales
static int menu_selection = 0;
static bool uart_ok = false;

// Función para inicializar UART (simulada)
void init_uart() {
    // En implementación real: configurar UART para comunicación con Pi
    uart_ok = true; // Simular que UART está OK
}

// Función para enviar mensaje (simulada)
bool send_to_pi(const char* problem) {
    // En implementación real: enviar via UART al Raspberry Pi
    // Formato: "PROBLEM:texto_del_problema\n"
    return uart_ok;
}

// Función para recibir respuesta (simulada)
bool receive_from_pi(char* response, int max_len) {
    // En implementación real: recibir respuesta del Pi
    // Formato esperado: "SOLUTION:respuesta_de_la_ia\n"
    if (uart_ok) {
        strcpy(response, "Premium: $45.67/month, Present Value: $8,234.56");
        return true;
    }
    return false;
}

// Función principal de la aplicación
void extapp_main() {
    // Inicializar UART
    init_uart();
    
    // Pantalla principal
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
    
    // Título
    extapp_drawTextLarge("ActuarialAI", 80, 20, BLUE, WHITE, false);
    extapp_drawTextSmall("AI Calculator for Actuarial Science", 40, 45, BLACK, WHITE, false);
    
    // Estado de conexión
    if (uart_ok) {
        extapp_drawTextSmall("Status: Connected to Pi", 10, 70, GREEN, WHITE, false);
    } else {
        extapp_drawTextSmall("Status: UART Error", 10, 70, RED, WHITE, false);
    }
    
    // Menú de opciones
    extapp_drawTextSmall("1. Life Insurance Premium", 10, 100, BLACK, WHITE, false);
    extapp_drawTextSmall("2. Annuity Present Value", 10, 120, BLACK, WHITE, false);
    extapp_drawTextSmall("3. Mortality Rate Lookup", 10, 140, BLACK, WHITE, false);
    extapp_drawTextSmall("4. Interest Calculation", 10, 160, BLACK, WHITE, false);
    
    // Instrucciones
    extapp_drawTextSmall("Press OK to calculate", 10, 190, BLUE, WHITE, false);
    extapp_drawTextSmall("Press Back to exit", 10, 210, BLACK, WHITE, false);
    
    // Bucle principal
    while (true) {
        uint64_t keys = extapp_scanKeyboard();
        
        if (keys & SCANCODE_OK || keys & SCANCODE_EXE) {
            // Pantalla de procesamiento
            extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
            extapp_drawTextLarge("Processing...", 70, 80, BLUE, WHITE, false);
            extapp_drawTextSmall("Sending to Google Cloud AI", 50, 110, BLACK, WHITE, false);
            extapp_drawTextSmall("Please wait...", 90, 130, BLACK, WHITE, false);
            
            extapp_msleep(2000); // Simular procesamiento
            
            // Enviar problema a la IA
            const char* problem = "Calculate life insurance premium for age 35, $100,000 coverage";
            char response[256];
            
            if (send_to_pi(problem) && receive_from_pi(response, sizeof(response))) {
                // Mostrar resultado
                extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
                extapp_drawTextLarge("AI Result:", 80, 20, GREEN, WHITE, false);
                
                // Dividir respuesta en líneas
                extapp_drawTextSmall("Life Insurance Calculation:", 10, 60, BLACK, WHITE, false);
                extapp_drawTextSmall(response, 10, 80, BLACK, WHITE, false);
                
                extapp_drawTextSmall("Based on:", 10, 110, BLACK, WHITE, false);
                extapp_drawTextSmall("- Standard mortality tables", 10, 130, BLACK, WHITE, false);
                extapp_drawTextSmall("- 3% interest rate", 10, 150, BLACK, WHITE, false);
                extapp_drawTextSmall("- 20-year term policy", 10, 170, BLACK, WHITE, false);
                
                extapp_drawTextSmall("Press any key to continue", 10, 210, BLUE, WHITE, false);
                
                // Esperar tecla
                while (true) {
                    uint64_t result_keys = extapp_scanKeyboard();
                    if (result_keys != 0) break;
                    extapp_msleep(100);
                }
            } else {
                // Error
                extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
                extapp_drawTextLarge("Error", 120, 80, RED, WHITE, false);
                extapp_drawTextSmall("Could not connect to Pi", 70, 110, RED, WHITE, false);
                extapp_drawTextSmall("Check UART connection", 70, 130, BLACK, WHITE, false);
                
                extapp_msleep(3000);
            }
            
            // Volver al menú principal
            extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
            extapp_drawTextLarge("ActuarialAI", 80, 20, BLUE, WHITE, false);
            extapp_drawTextSmall("AI Calculator for Actuarial Science", 40, 45, BLACK, WHITE, false);
            
            if (uart_ok) {
                extapp_drawTextSmall("Status: Connected to Pi", 10, 70, GREEN, WHITE, false);
            } else {
                extapp_drawTextSmall("Status: UART Error", 10, 70, RED, WHITE, false);
            }
            
            extapp_drawTextSmall("1. Life Insurance Premium", 10, 100, BLACK, WHITE, false);
            extapp_drawTextSmall("2. Annuity Present Value", 10, 120, BLACK, WHITE, false);
            extapp_drawTextSmall("3. Mortality Rate Lookup", 10, 140, BLACK, WHITE, false);
            extapp_drawTextSmall("4. Interest Calculation", 10, 160, BLACK, WHITE, false);
            
            extapp_drawTextSmall("Press OK to calculate", 10, 190, BLUE, WHITE, false);
            extapp_drawTextSmall("Press Back to exit", 10, 210, BLACK, WHITE, false);
            
        } else if (keys & SCANCODE_Back || keys & SCANCODE_Home) {
            // Salir
            break;
        }
        
        extapp_msleep(100);
    }
    
    // Pantalla de despedida
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
    extapp_drawTextLarge("Goodbye!", 100, 100, BLUE, WHITE, false);
    extapp_msleep(1000);
} 