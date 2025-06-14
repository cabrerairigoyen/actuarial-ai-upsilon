#include <extapp_api.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Colores
#define COLOR_WHITE     0xFFFF
#define COLOR_BLACK     0x0000
#define COLOR_BLUE      0x001F
#define COLOR_GREEN     0x07E0
#define COLOR_RED       0xF800
#define COLOR_CYAN      0x07FF
#define COLOR_YELLOW    0xFFE0
#define COLOR_GRAY      0x8410

// Estados de la aplicación
typedef enum {
    STATE_MENU,
    STATE_INPUT,
    STATE_PROCESSING,
    STATE_RESULT,
    STATE_TEST
} AppState;

// Opciones del menú
typedef enum {
    MENU_LIFE_INSURANCE = 0,
    MENU_ANNUITIES = 1,
    MENU_MORTALITY = 2,
    MENU_INTEREST = 3,
    MENU_CUSTOM = 4,
    MENU_TEST = 5,
    MENU_EXIT = 6,
    MENU_COUNT = 7
} MenuOption;

// Variables globales
static AppState current_state = STATE_MENU;
static int selected_menu = 0;
static char response_buffer[1024];
static bool uart_available = false;

// Textos del menú
static const char* menu_items[] = {
    "1. Life Insurance",
    "2. Annuities", 
    "3. Mortality Tables",
    "4. Interest Calculations",
    "5. Custom Problem",
    "6. Test Connection",
    "7. Exit"
};

// Plantillas de problemas
static const char* get_problem_template(MenuOption option) {
    switch (option) {
        case MENU_LIFE_INSURANCE:
            return "Calculate life insurance premium for age 35, $100,000 coverage, 20-year term";
        case MENU_ANNUITIES:
            return "Calculate present value of annuity: $1000/month for 20 years at 5% interest";
        case MENU_MORTALITY:
            return "Find mortality rate for age 45 using standard mortality table";
        case MENU_INTEREST:
            return "Calculate compound interest: $10,000 at 6% for 15 years";
        case MENU_CUSTOM:
            return "Enter your actuarial problem";
        default:
            return "Test connection";
    }
}

// Funciones de comunicación UART (simuladas por ahora)
static bool uart_init() {
    // En una implementación real, aquí inicializarías UART
    // Por ahora simulamos que está disponible
    uart_available = true;
    return true;
}

static bool uart_send_string(const char* str) {
    // Simulación de envío UART
    // En implementación real: enviar via UART a Raspberry Pi
    return uart_available;
}

static bool uart_receive_string(char* buffer, int max_length, int timeout_ms) {
    // Simulación de recepción UART
    // En implementación real: recibir respuesta del Pi
    if (!uart_available) {
        strcpy(buffer, "UART not available");
        return false;
    }
    
    // Respuesta simulada para demostración
    strcpy(buffer, "SOLUTION: Premium calculation: $45.67/month based on mortality tables and 3% interest rate. Present value: $8,234.56");
    return true;
}

// Funciones de interfaz
static void clear_screen() {
    extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, COLOR_WHITE);
}

static void draw_title() {
    extapp_drawTextLarge("ActuarialAI", 80, 10, COLOR_BLUE, COLOR_WHITE, false);
    extapp_drawTextSmall("Raspberry Pi + Google Cloud", 60, 35, COLOR_GRAY, COLOR_WHITE, false);
}

static void draw_menu() {
    clear_screen();
    draw_title();
    
    // Estado UART
    if (uart_available) {
        extapp_drawTextSmall("UART: OK", 10, 60, COLOR_GREEN, COLOR_WHITE, false);
    } else {
        extapp_drawTextSmall("UART: ERROR", 10, 60, COLOR_RED, COLOR_WHITE, false);
    }
    
    // Opciones del menú
    for (int i = 0; i < MENU_COUNT; i++) {
        uint16_t color = (i == selected_menu) ? COLOR_BLACK : COLOR_GRAY;
        uint16_t bg = (i == selected_menu) ? COLOR_CYAN : COLOR_WHITE;
        
        extapp_drawTextSmall(menu_items[i], 10, 85 + i * 18, color, bg, false);
    }
    
    // Instrucciones
    extapp_drawTextSmall("Use arrows and OK", 10, 210, COLOR_GRAY, COLOR_WHITE, false);
}

static void draw_input_screen(const char* title) {
    clear_screen();
    draw_title();
    
    extapp_drawTextSmall(title, 10, 60, COLOR_BLACK, COLOR_WHITE, false);
    extapp_drawTextSmall("Problem template:", 10, 85, COLOR_BLACK, COLOR_WHITE, false);
    
    // Mostrar plantilla del problema
    const char* template = get_problem_template((MenuOption)selected_menu);
    
    // Dividir texto en líneas
    char temp[256];
    strncpy(temp, template, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    
    char* line = temp;
    int y = 110;
    int line_length = 35; // Caracteres por línea
    
    while (strlen(line) > 0 && y < 180) {
        char display_line[40];
        if (strlen(line) <= line_length) {
            strcpy(display_line, line);
            line += strlen(line);
        } else {
            strncpy(display_line, line, line_length);
            display_line[line_length] = '\0';
            line += line_length;
        }
        
        extapp_drawTextSmall(display_line, 10, y, COLOR_BLACK, COLOR_WHITE, false);
        y += 15;
    }
    
    extapp_drawTextSmall("Press OK to send", 10, 190, COLOR_BLUE, COLOR_WHITE, false);
    extapp_drawTextSmall("Press Back to return", 10, 210, COLOR_GRAY, COLOR_WHITE, false);
}

static void draw_processing_screen() {
    clear_screen();
    draw_title();
    
    extapp_drawTextLarge("Processing...", 70, 100, COLOR_BLUE, COLOR_WHITE, false);
    extapp_drawTextSmall("Sending to Google Cloud AI", 50, 130, COLOR_GRAY, COLOR_WHITE, false);
    extapp_drawTextSmall("Please wait...", 90, 150, COLOR_GRAY, COLOR_WHITE, false);
}

static void draw_result_screen(const char* result) {
    clear_screen();
    draw_title();
    
    extapp_drawTextSmall("AI Response:", 10, 60, COLOR_BLACK, COLOR_WHITE, false);
    
    // Mostrar resultado dividido en líneas
    char temp[1024];
    strncpy(temp, result, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    
    char* line = temp;
    int y = 85;
    int line_length = 35; // Caracteres por línea
    
    while (strlen(line) > 0 && y < 180) {
        char display_line[40];
        if (strlen(line) <= line_length) {
            strcpy(display_line, line);
            line += strlen(line);
        } else {
            // Buscar espacio para cortar palabra completa
            int cut_pos = line_length;
            while (cut_pos > 0 && line[cut_pos] != ' ') {
                cut_pos--;
            }
            if (cut_pos == 0) cut_pos = line_length;
            
            strncpy(display_line, line, cut_pos);
            display_line[cut_pos] = '\0';
            line += cut_pos;
            if (*line == ' ') line++; // Saltar espacio
        }
        
        extapp_drawTextSmall(display_line, 10, y, COLOR_BLACK, COLOR_WHITE, false);
        y += 15;
    }
    
    extapp_drawTextSmall("Press any key to continue", 10, 210, COLOR_GRAY, COLOR_WHITE, false);
}

static void draw_test_screen() {
    clear_screen();
    draw_title();
    
    extapp_drawTextLarge("Testing...", 90, 100, COLOR_BLUE, COLOR_WHITE, false);
    extapp_drawTextSmall("Checking connection to Pi", 60, 130, COLOR_GRAY, COLOR_WHITE, false);
}

// Función principal de comunicación con IA
static bool send_problem_to_ai(const char* problem) {
    char message[512];
    snprintf(message, sizeof(message), "PROBLEM:%s\n", problem);
    
    // Enviar a Raspberry Pi
    if (!uart_send_string(message)) {
        strcpy(response_buffer, "Error: Failed to send problem");
        return false;
    }
    
    // Esperar respuesta
    if (!uart_receive_string(response_buffer, sizeof(response_buffer), 30000)) {
        strcpy(response_buffer, "Error: No response from AI (timeout)");
        return false;
    }
    
    // Verificar formato de respuesta
    if (strncmp(response_buffer, "SOLUTION:", 9) == 0) {
        // Remover prefijo "SOLUTION:"
        memmove(response_buffer, response_buffer + 9, strlen(response_buffer) - 8);
        return true;
    }
    
    return true; // Para demostración, siempre exitoso
}

// Función de prueba de conexión
static bool test_connection() {
    if (!uart_send_string("TEST_CONNECTION\n")) {
        return false;
    }
    
    char test_response[256];
    if (!uart_receive_string(test_response, sizeof(test_response), 5000)) {
        return false;
    }
    
    return (strstr(test_response, "TEST_OK") != NULL);
}

// Manejo de entrada de teclado
static void handle_menu_input() {
    uint64_t keys = extapp_scanKeyboard();
    
    if (keys & SCANCODE_Up && selected_menu > 0) {
        selected_menu--;
    } else if (keys & SCANCODE_Down && selected_menu < MENU_COUNT - 1) {
        selected_menu++;
    } else if (keys & SCANCODE_OK || keys & SCANCODE_EXE) {
        if (selected_menu == MENU_EXIT) {
            return; // Salir de la aplicación
        } else if (selected_menu == MENU_TEST) {
            current_state = STATE_TEST;
        } else {
            current_state = STATE_INPUT;
        }
    }
}

static void handle_input_screen() {
    uint64_t keys = extapp_scanKeyboard();
    
    if (keys & SCANCODE_OK || keys & SCANCODE_EXE) {
        current_state = STATE_PROCESSING;
    } else if (keys & SCANCODE_Back) {
        current_state = STATE_MENU;
    }
}

static void handle_processing() {
    const char* problem = get_problem_template((MenuOption)selected_menu);
    
    if (send_problem_to_ai(problem)) {
        current_state = STATE_RESULT;
    } else {
        current_state = STATE_RESULT; // Mostrar error
    }
}

static void handle_result_screen() {
    uint64_t keys = extapp_scanKeyboard();
    
    if (keys != 0) { // Cualquier tecla
        current_state = STATE_MENU;
    }
}

static void handle_test_screen() {
    static bool test_started = false;
    static uint64_t test_start_time = 0;
    
    if (!test_started) {
        test_started = true;
        test_start_time = extapp_millis();
        return;
    }
    
    // Simular prueba de 2 segundos
    if (extapp_millis() - test_start_time > 2000) {
        bool connection_ok = test_connection();
        
        if (connection_ok) {
            strcpy(response_buffer, "Connection test successful!\nRaspberry Pi responding correctly.");
        } else {
            strcpy(response_buffer, "Connection test failed!\nCheck UART wiring and Pi status.");
        }
        
        test_started = false;
        current_state = STATE_RESULT;
    }
}

// Función principal de la aplicación
void extapp_main() {
    // Inicializar UART
    uart_init();
    
    // Bucle principal
    while (true) {
        switch (current_state) {
            case STATE_MENU:
                draw_menu();
                extapp_msleep(100);
                handle_menu_input();
                break;
                
            case STATE_INPUT:
                draw_input_screen(menu_items[selected_menu]);
                extapp_msleep(100);
                handle_input_screen();
                break;
                
            case STATE_PROCESSING:
                draw_processing_screen();
                extapp_msleep(500); // Mostrar pantalla de procesamiento
                handle_processing();
                break;
                
            case STATE_RESULT:
                draw_result_screen(response_buffer);
                extapp_msleep(100);
                handle_result_screen();
                break;
                
            case STATE_TEST:
                draw_test_screen();
                extapp_msleep(100);
                handle_test_screen();
                break;
        }
        
        // Verificar si se presiona Back para salir
        uint64_t keys = extapp_scanKeyboard();
        if (keys & SCANCODE_Home) {
            break; // Salir de la aplicación
        }
    }
} 