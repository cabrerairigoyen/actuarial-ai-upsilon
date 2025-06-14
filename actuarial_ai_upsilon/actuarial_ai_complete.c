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

// Estados de la aplicación
typedef enum {
    STATE_INIT,
    STATE_MENU,
    STATE_PROCESSING,
    STATE_RESULT,
    STATE_ERROR,
    STATE_TEST
} AppState;

// Variables globales
static AppState current_state = STATE_INIT;
static int menu_selection = 0;
static char response_buffer[1024];
static bool uart_ready = false;

// Problemas actuariales predefinidos
static const char* problems[] = {
    "Calculate life insurance premium for age 35, $100,000 coverage, 20-year term",
    "Calculate present value of annuity: $1000/month for 20 years at 5% interest", 
    "Find mortality rate for age 45 using standard mortality table",
    "Calculate compound interest: $10,000 at 6% for 15 years",
    "Calculate reserves for whole life insurance policy age 30",
    "TEST_CONNECTION"
};

static const char* problem_names[] = {
    "Life Insurance Premium",
    "Annuity Present Value", 
    "Mortality Rate Lookup",
    "Interest Calculation",
    "Insurance Reserves",
    "Test Connection"
};

// Declaraciones de funciones UART (implementadas en uart_hardware.c)
extern bool uart_hardware_init(void);
extern bool uart_hardware_send_string(const char* str);
extern bool uart_hardware_receive_string(char* buffer, int max_len, uint32_t timeout_ms);
extern bool uart_hardware_test_loopback(void);

// Funciones UART de alto nivel
static bool uart_init() {
    uart_ready = uart_hardware_init();
    return uart_ready;
}

static bool send_problem_to_pi(const char* problem) {
    if (!uart_ready || !problem) return false;
    
    // Formatear mensaje para el protocolo
    char message[512];
    snprintf(message, sizeof(message), "PROBLEM:%s\n", problem);
    
    // Enviar al Raspberry Pi
    if (!uart_hardware_send_string(message)) {
        strcpy(response_buffer, "Error: Failed to send to Pi");
        return false;
    }
    
    // Recibir respuesta con timeout de 30 segundos
    if (!uart_hardware_receive_string(response_buffer, sizeof(response_buffer), 30000)) {
        strcpy(response_buffer, "Error: No response from Pi (timeout)");
        return false;
    }
    
    // Procesar respuesta
    if (strncmp(response_buffer, "SOLUTION:", 9) == 0) {
        // Remover prefijo "SOLUTION:"
        memmove(response_buffer, response_buffer + 9, strlen(response_buffer) - 8);
    }
    
    return true;
}

static bool test_pi_connection() {
    if (!uart_ready) return false;
    
    if (!uart_hardware_send_string("TEST_CONNECTION\n")) {
        return false;
    }
    
    char test_response[256];
    if (!uart_hardware_receive_string(test_response, sizeof(test_response), 5000)) {
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
    extapp_drawTextSmall("Native UART App", 90, 35, BLACK, WHITE, false);
}

static void draw_status() {
    if (uart_ready) {
        extapp_drawTextSmall("UART: Ready (115200)", 10, 55, GREEN, WHITE, false);
        extapp_drawTextSmall("Pins: PA11(TX) PA12(RX)", 10, 70, GREEN, WHITE, false);
    } else {
        extapp_drawTextSmall("UART: Not initialized", 10, 55, RED, WHITE, false);
    }
}

static void draw_init_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextLarge("Initializing...", 70, 100, BLUE, WHITE, false);
    extapp_drawTextSmall("Configuring UART hardware", 50, 130, BLACK, WHITE, false);
    extapp_drawTextSmall("STM32F730 UART1 registers", 50, 150, BLACK, WHITE, false);
    extapp_drawTextSmall("Baudrate: 115200 baud", 70, 170, BLACK, WHITE, false);
}

static void draw_menu() {
    clear_screen();
    draw_header();
    draw_status();
    
    extapp_drawTextSmall("Select calculation:", 10, 90, BLACK, WHITE, false);
    
    // Dibujar opciones del menú
    for (int i = 0; i < 6; i++) {
        uint16_t color = (i == menu_selection) ? WHITE : BLACK;
        uint16_t bg = (i == menu_selection) ? BLUE : WHITE;
        
        char menu_line[50];
        snprintf(menu_line, sizeof(menu_line), "%d. %s", i + 1, problem_names[i]);
        extapp_drawTextSmall(menu_line, 10, 110 + i * 16, color, bg, false);
    }
    
    // Instrucciones
    extapp_drawTextSmall("Up/Down: Navigate  OK: Select", 10, 200, BLACK, WHITE, false);
    extapp_drawTextSmall("Back: Exit", 10, 220, BLACK, WHITE, false);
}

static void draw_processing_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextLarge("Processing...", 70, 80, BLUE, WHITE, false);
    extapp_drawTextSmall("Sending via UART to Pi", 60, 110, BLACK, WHITE, false);
    extapp_drawTextSmall("Pi forwarding to Google Cloud", 40, 130, BLACK, WHITE, false);
    extapp_drawTextSmall("Waiting for AI response...", 60, 150, BLACK, WHITE, false);
    
    // Animación simple
    static int dots = 0;
    dots = (dots + 1) % 4;
    
    char progress[20] = "Working";
    for (int i = 0; i < dots; i++) {
        strcat(progress, ".");
    }
    
    extapp_drawTextSmall(progress, 120, 170, BLUE, WHITE, false);
}

static void draw_result_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextSmall("AI Response:", 10, 60, GREEN, WHITE, false);
    
    // Mostrar resultado dividido en líneas
    char temp_buffer[1024];
    strncpy(temp_buffer, response_buffer, sizeof(temp_buffer) - 1);
    temp_buffer[sizeof(temp_buffer) - 1] = '\0';
    
    char* line = temp_buffer;
    int y = 80;
    int chars_per_line = 38;
    
    while (strlen(line) > 0 && y < 190) {
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
        y += 14;
    }
    
    extapp_drawTextSmall("Powered by Google Cloud AI", 10, 200, BLUE, WHITE, false);
    extapp_drawTextSmall("Press any key to continue", 10, 220, BLACK, WHITE, false);
}

static void draw_error_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextLarge("Error", 130, 80, RED, WHITE, false);
    
    // Mostrar mensaje de error
    char temp_buffer[1024];
    strncpy(temp_buffer, response_buffer, sizeof(temp_buffer) - 1);
    temp_buffer[sizeof(temp_buffer) - 1] = '\0';
    
    char* line = temp_buffer;
    int y = 110;
    
    while (strlen(line) > 0 && y < 180) {
        char display_line[35];
        if (strlen(line) <= 34) {
            strcpy(display_line, line);
            line += strlen(line);
        } else {
            strncpy(display_line, line, 34);
            display_line[34] = '\0';
            line += 34;
        }
        
        extapp_drawTextSmall(display_line, 10, y, RED, WHITE, false);
        y += 15;
    }
    
    extapp_drawTextSmall("Check connections and try again", 10, 200, BLACK, WHITE, false);
    extapp_drawTextSmall("Press any key to continue", 10, 220, BLACK, WHITE, false);
}

static void draw_test_screen() {
    clear_screen();
    draw_header();
    
    extapp_drawTextLarge("Testing...", 90, 100, BLUE, WHITE, false);
    extapp_drawTextSmall("Checking Pi connection", 70, 130, BLACK, WHITE, false);
    extapp_drawTextSmall("Sending TEST_CONNECTION", 60, 150, BLACK, WHITE, false);
}

// Función principal
void extapp_main() {
    uint64_t last_update = 0;
    bool processing_started = false;
    
    while (true) {
        uint64_t current_time = extapp_millis();
        uint64_t keys = extapp_scanKeyboard();
        
        switch (current_state) {
            case STATE_INIT:
                draw_init_screen();
                extapp_msleep(2000);  // Mostrar pantalla de inicialización
                
                if (uart_init()) {
                    current_state = STATE_MENU;
                } else {
                    strcpy(response_buffer, "Failed to initialize UART hardware");
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
                    extapp_msleep(150);
                } else if (keys & SCANCODE_Down && menu_selection < 5) {
                    menu_selection++;
                    extapp_msleep(150);
                } else if (keys & SCANCODE_OK || keys & SCANCODE_EXE) {
                    if (menu_selection == 5) {  // Test connection
                        current_state = STATE_TEST;
                    } else {
                        current_state = STATE_PROCESSING;
                        processing_started = false;
                    }
                } else if (keys & SCANCODE_Back || keys & SCANCODE_Home) {
                    // Pantalla de despedida
                    clear_screen();
                    extapp_drawTextLarge("Goodbye!", 100, 100, BLUE, WHITE, false);
                    extapp_msleep(1000);
                    return;
                }
                break;
                
            case STATE_PROCESSING:
                if (current_time - last_update > 300) {
                    draw_processing_screen();
                    last_update = current_time;
                }
                
                if (!processing_started) {
                    processing_started = true;
                    
                    if (send_problem_to_pi(problems[menu_selection])) {
                        current_state = STATE_RESULT;
                    } else {
                        current_state = STATE_ERROR;
                    }
                }
                break;
                
            case STATE_TEST:
                draw_test_screen();
                extapp_msleep(1000);
                
                if (test_pi_connection()) {
                    strcpy(response_buffer, "Connection test successful! Raspberry Pi is responding correctly via UART.");
                    current_state = STATE_RESULT;
                } else {
                    strcpy(response_buffer, "Connection test failed. Check UART wiring and Pi status.");
                    current_state = STATE_ERROR;
                }
                break;
                
            case STATE_RESULT:
                if (current_time - last_update > 100) {
                    draw_result_screen();
                    last_update = current_time;
                }
                
                if (keys != 0) {
                    current_state = STATE_MENU;
                    extapp_msleep(200);
                }
                break;
                
            case STATE_ERROR:
                if (current_time - last_update > 100) {
                    draw_error_screen();
                    last_update = current_time;
                }
                
                if (keys != 0) {
                    current_state = STATE_MENU;
                    extapp_msleep(200);
                }
                break;
        }
        
        extapp_msleep(50);
    }
} 