#include <extapp_api.h>
#include <string.h>
#include <stdio.h>

// Colores
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE  0x001F
#define GREEN 0x07E0
#define RED   0xF800
#define CYAN  0x07FF
#define YELLOW 0xFFE0

// Configuración UART
#define UART_BAUDRATE 115200
#define UART_TX_PIN 11  // PA11
#define UART_RX_PIN 12  // PA12
#define MAX_RESPONSE_SIZE 1024
#define UART_TIMEOUT_MS 30000

// Estados de la aplicación
typedef enum {
    STATE_INIT,
    STATE_MENU,
    STATE_PROCESSING,
    STATE_RESULT,
    STATE_ERROR
} AppState;

// Variables globales
static AppState current_state = STATE_INIT;
static int menu_selection = 0;
static char response_buffer[MAX_RESPONSE_SIZE];
static bool uart_initialized = false;
static bool uart_ok = false;

// Problemas predefinidos
static const char* problems[] = {
    "Calculate life insurance premium for age 35, $100,000 coverage, 20-year term",
    "Calculate present value of annuity: $1000/month for 20 years at 5% interest", 
    "Find mortality rate for age 45 using standard mortality table",
    "Calculate compound interest: $10,000 at 6% for 15 years",
    "Calculate reserves for whole life insurance policy age 30"
};

static const char* problem_names[] = {
    "Life Insurance Premium",
    "Annuity Present Value", 
    "Mortality Rate Lookup",
    "Interest Calculation",
    "Insurance Reserves"
};

// Funciones UART (estructura para implementación real)
bool uart_init() {
    // TODO: Implementar configuración UART real
    // - Configurar pines PA11 (TX) y PA12 (RX)
    // - Establecer baudrate 115200
    // - Configurar formato: 8N1
    uart_initialized = true;
    uart_ok = true; // Simular éxito por ahora
    return true;
}

bool uart_send_string(const char* str) {
    // TODO: Implementar envío UART real
    // - Enviar cada byte por UART
    // - Verificar que la transmisión sea exitosa
    if (!uart_initialized || !str) return false;
    
    while (*str) {
        if (!uart_send_byte(*str)) return false;
        str++;
    }
    
    return true;
}

bool uart_receive_string(char* buffer, int max_len, int timeout_ms) {
    // TODO: Implementar recepción UART real
    // - Leer bytes hasta encontrar '\n'
    // - Manejar timeout
    // - Verificar formato "SOLUTION:..."
    
    if (uart_ok) {
        // Respuesta simulada para demostración
        strcpy(buffer, "SOLUTION:Premium: $45.67/month, Present Value: $8,234.56, Risk: Low");
        return true;
    }
    return false;
}

// Funciones UART reales
static bool uart_send_byte(uint8_t byte) {
    if (!uart_initialized) return false;
    
    // En implementación real:
    // - Esperar que el registro de transmisión esté vacío
    // - Escribir byte al registro de datos UART
    // - Esperar confirmación de transmisión
    
    // Simulación para demostración
    return true;
}

static bool uart_receive_byte(uint8_t* byte, uint32_t timeout_ms) {
    if (!uart_initialized || !byte) return false;
    
    uint64_t start_time = extapp_millis();
    
    // En implementación real:
    // - Verificar si hay datos disponibles en el buffer de recepción
    // - Leer byte del registro de datos UART
    // - Manejar timeout
    
    // Simulación: esperar un poco y devolver datos simulados
    while (extapp_millis() - start_time < timeout_ms) {
        extapp_msleep(10);
        // Simular recepción de datos después de 2 segundos
        if (extapp_millis() - start_time > 2000) {
            *byte = 'S'; // Primer byte de respuesta simulada
            return true;
        }
    }
    
    return false; // Timeout
}

// Funciones de comunicación con IA
static bool send_problem_to_ai(const char* problem) {
    if (!problem) return false;
    
    // Formatear mensaje
    char message[512];
    snprintf(message, sizeof(message), "PROBLEM:%s\n", problem);
    
    // Enviar al Raspberry Pi
    if (!uart_send_string(message)) {
        strcpy(response_buffer, "Error: Failed to send problem to Pi");
        return false;
    }
    
    // Recibir respuesta
    if (!uart_receive_string(response_buffer, sizeof(response_buffer), UART_TIMEOUT_MS)) {
        strcpy(response_buffer, "Error: No response from Pi (timeout after 30s)");
        return false;
    }
    
    // Verificar formato de respuesta
    if (strncmp(response_buffer, "SOLUTION:", 9) == 0) {
        // Remover prefijo "SOLUTION:"
        memmove(response_buffer, response_buffer + 9, strlen(response_buffer) - 8);
        return true;
    }
    
    return true; // Aceptar cualquier respuesta para demostración
}

static bool test_uart_connection() {
    if (!uart_send_string("TEST_CONNECTION\n")) {
        return false;
    }
    
    char test_response[256];
    if (!uart_receive_string(test_response, sizeof(test_response), 5000)) {
        return false;
    }
    
    return (strstr(test_response, "TEST_OK") != NULL);
}

// Funciones de interfaz
static void clear_screen() {
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
}

static void draw_header() {
    extapp_drawTextLarge("ActuarialAI", 80, 10, BLUE, WHITE, false);
    extapp_drawTextSmall("Native UART Communication", 60, 35, BLACK, WHITE, false);
}

static void draw_uart_status() {
    if (uart_initialized) {
        extapp_drawTextSmall("UART: Initialized", 10, 55, GREEN, WHITE, false);
    } else {
        extapp_drawTextSmall("UART: Error", 10, 55, RED, WHITE, false);
    }
}

static void draw_init_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextLarge("Initializing...", 70, 100, BLUE, WHITE, false);
    extapp_drawTextSmall("Setting up UART communication", 50, 130, BLACK, WHITE, false);
    extapp_drawTextSmall("Baudrate: 115200", 90, 150, BLACK, WHITE, false);
    extapp_drawTextSmall("Pins: PA11(TX), PA12(RX)", 70, 170, BLACK, WHITE, false);
}

static void draw_menu() {
    clear_screen();
    draw_header();
    draw_uart_status();
    
    extapp_drawTextSmall("Select actuarial calculation:", 10, 80, BLACK, WHITE, false);
    
    // Dibujar opciones del menú
    for (int i = 0; i < 5; i++) {
        uint16_t color = (i == menu_selection) ? WHITE : BLACK;
        uint16_t bg = (i == menu_selection) ? BLUE : WHITE;
        
        char menu_line[50];
        snprintf(menu_line, sizeof(menu_line), "%d. %s", i + 1, problem_names[i]);
        extapp_drawTextSmall(menu_line, 10, 100 + i * 18, color, bg, false);
    }
    
    // Instrucciones
    extapp_drawTextSmall("Up/Down: Navigate", 10, 200, BLACK, WHITE, false);
    extapp_drawTextSmall("OK: Calculate  Back: Exit", 10, 220, BLACK, WHITE, false);
}

static void draw_processing_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextLarge("Processing...", 70, 80, BLUE, WHITE, false);
    extapp_drawTextSmall("Sending to Raspberry Pi", 60, 110, BLACK, WHITE, false);
    extapp_drawTextSmall("Waiting for AI response", 60, 130, BLACK, WHITE, false);
    
    // Indicador de progreso simple
    static int progress = 0;
    progress = (progress + 1) % 4;
    
    char progress_str[10] = "Working";
    for (int i = 0; i < progress; i++) {
        strcat(progress_str, ".");
    }
    
    extapp_drawTextSmall(progress_str, 120, 150, BLUE, WHITE, false);
}

static void draw_result_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextSmall("AI Response:", 10, 60, GREEN, WHITE, false);
    
    // Dividir respuesta en líneas para mostrar
    char temp_buffer[MAX_RESPONSE_SIZE];
    strncpy(temp_buffer, response_buffer, sizeof(temp_buffer) - 1);
    temp_buffer[sizeof(temp_buffer) - 1] = '\0';
    
    char* line = temp_buffer;
    int y = 80;
    int chars_per_line = 38;
    
    while (strlen(line) > 0 && y < 200) {
        char display_line[40];
        
        if (strlen(line) <= chars_per_line) {
            strcpy(display_line, line);
            line += strlen(line);
        } else {
            // Buscar espacio para cortar palabra completa
            int cut_pos = chars_per_line;
            while (cut_pos > 0 && line[cut_pos] != ' ' && line[cut_pos] != '\0') {
                cut_pos--;
            }
            if (cut_pos == 0) cut_pos = chars_per_line;
            
            strncpy(display_line, line, cut_pos);
            display_line[cut_pos] = '\0';
            line += cut_pos;
            if (*line == ' ') line++; // Saltar espacio
        }
        
        extapp_drawTextSmall(display_line, 10, y, BLACK, WHITE, false);
        y += 15;
    }
    
    extapp_drawTextSmall("Press any key to continue", 10, 220, BLUE, WHITE, false);
}

static void draw_error_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextLarge("Error", 130, 80, RED, WHITE, false);
    extapp_drawTextSmall("Communication failed", 80, 110, RED, WHITE, false);
    extapp_drawTextSmall("Check connections:", 80, 130, BLACK, WHITE, false);
    extapp_drawTextSmall("- UART wiring", 80, 150, BLACK, WHITE, false);
    extapp_drawTextSmall("- Raspberry Pi status", 80, 170, BLACK, WHITE, false);
    
    extapp_drawTextSmall("Press any key to retry", 70, 210, BLUE, WHITE, false);
}

// Función principal
void extapp_main() {
    uint64_t last_update = 0;
    
    while (true) {
        uint64_t current_time = extapp_millis();
        uint64_t keys = extapp_scanKeyboard();
        
        switch (current_state) {
            case STATE_INIT:
                draw_init_screen();
                extapp_msleep(1000);
                
                if (uart_init()) {
                    current_state = STATE_MENU;
                } else {
                    strcpy(response_buffer, "Failed to initialize UART");
                    current_state = STATE_ERROR;
                }
                break;
                
            case STATE_MENU:
                if (current_time - last_update > 100) {
                    draw_menu();
                    last_update = current_time;
                }
                
                if (keys & SCANCODE_Up && menu_selection > 0) {
                    menu_selection--;
                } else if (keys & SCANCODE_Down && menu_selection < 4) {
                    menu_selection++;
                } else if (keys & SCANCODE_OK || keys & SCANCODE_EXE) {
                    current_state = STATE_PROCESSING;
                } else if (keys & SCANCODE_Back || keys & SCANCODE_Home) {
                    return; // Salir
                }
                break;
                
            case STATE_PROCESSING:
                if (current_time - last_update > 500) {
                    draw_processing_screen();
                    last_update = current_time;
                }
                
                // Procesar en background
                static bool processing_started = false;
                if (!processing_started) {
                    processing_started = true;
                    
                    if (send_problem_to_ai(problems[menu_selection])) {
                        current_state = STATE_RESULT;
                    } else {
                        current_state = STATE_ERROR;
                    }
                    processing_started = false;
                }
                break;
                
            case STATE_RESULT:
                if (current_time - last_update > 100) {
                    draw_result_screen();
                    last_update = current_time;
                }
                
                if (keys != 0) {
                    current_state = STATE_MENU;
                }
                break;
                
            case STATE_ERROR:
                if (current_time - last_update > 100) {
                    draw_error_screen();
                    last_update = current_time;
                }
                
                if (keys != 0) {
                    current_state = STATE_MENU;
                }
                break;
        }
        
        extapp_msleep(50);
    }
} 