// Implementación UART real para NumWorks (STM32F730)
// Este archivo contiene la implementación de bajo nivel para UART

#include <stdint.h>
#include <stdbool.h>

// Direcciones de registros UART1 (STM32F730)
#define UART1_BASE      0x40011000
#define RCC_BASE        0x40023800
#define GPIOA_BASE      0x40020000

// Registros UART1
#define UART1_CR1       (*(volatile uint32_t*)(UART1_BASE + 0x00))
#define UART1_CR2       (*(volatile uint32_t*)(UART1_BASE + 0x04))
#define UART1_CR3       (*(volatile uint32_t*)(UART1_BASE + 0x08))
#define UART1_BRR       (*(volatile uint32_t*)(UART1_BASE + 0x0C))
#define UART1_ISR       (*(volatile uint32_t*)(UART1_BASE + 0x1C))
#define UART1_ICR       (*(volatile uint32_t*)(UART1_BASE + 0x20))
#define UART1_RDR       (*(volatile uint32_t*)(UART1_BASE + 0x24))
#define UART1_TDR       (*(volatile uint32_t*)(UART1_BASE + 0x28))

// Registros RCC (Reset and Clock Control)
#define RCC_AHB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB2ENR     (*(volatile uint32_t*)(RCC_BASE + 0x44))

// Registros GPIO Puerto A
#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_AFRL      (*(volatile uint32_t*)(GPIOA_BASE + 0x20))
#define GPIOA_AFRH      (*(volatile uint32_t*)(GPIOA_BASE + 0x24))

// Bits de control UART
#define UART_CR1_UE     (1 << 0)   // UART Enable
#define UART_CR1_RE     (1 << 2)   // Receiver Enable
#define UART_CR1_TE     (1 << 3)   // Transmitter Enable
#define UART_ISR_TXE    (1 << 7)   // Transmit Data Register Empty
#define UART_ISR_RXNE   (1 << 5)   // Read Data Register Not Empty
#define UART_ISR_TC     (1 << 6)   // Transmission Complete

// Configuración de pines
#define GPIO_AF7        0x07       // Función alternativa 7 para UART1

bool uart_hardware_init(void) {
    // 1. Habilitar clocks
    RCC_AHB1ENR |= (1 << 0);  // Habilitar clock GPIOA
    RCC_APB2ENR |= (1 << 4);  // Habilitar clock UART1
    
    // 2. Configurar pines PA11 (TX) y PA12 (RX)
    // PA11 y PA12 como función alternativa
    GPIOA_MODER &= ~((3 << 22) | (3 << 24));  // Limpiar bits
    GPIOA_MODER |= (2 << 22) | (2 << 24);     // Función alternativa
    
    // Configurar función alternativa AF7 para UART1
    GPIOA_AFRH &= ~((0xF << 12) | (0xF << 16));  // Limpiar AF11 y AF12
    GPIOA_AFRH |= (GPIO_AF7 << 12) | (GPIO_AF7 << 16);  // AF7 para PA11 y PA12
    
    // 3. Configurar UART1
    UART1_CR1 = 0;  // Deshabilitar UART durante configuración
    
    // Configurar baudrate 115200
    // Asumiendo clock de 216 MHz: BRR = 216000000 / 115200 ≈ 1875
    UART1_BRR = 1875;
    
    // Configurar formato: 8 bits, sin paridad, 1 bit de stop (por defecto)
    UART1_CR2 = 0;
    UART1_CR3 = 0;
    
    // Habilitar transmisor, receptor y UART
    UART1_CR1 = UART_CR1_UE | UART_CR1_RE | UART_CR1_TE;
    
    return true;
}

bool uart_hardware_send_byte(uint8_t byte) {
    // Esperar que el registro de transmisión esté vacío
    uint32_t timeout = 100000;
    while (!(UART1_ISR & UART_ISR_TXE) && timeout--) {
        // Esperar
    }
    
    if (timeout == 0) return false;  // Timeout
    
    // Enviar byte
    UART1_TDR = byte;
    
    // Esperar que la transmisión se complete
    timeout = 100000;
    while (!(UART1_ISR & UART_ISR_TC) && timeout--) {
        // Esperar
    }
    
    return timeout > 0;
}

bool uart_hardware_receive_byte(uint8_t* byte, uint32_t timeout_ms) {
    if (!byte) return false;
    
    // Convertir timeout a ciclos aproximados
    uint32_t timeout_cycles = timeout_ms * 1000;  // Aproximación
    
    // Esperar datos disponibles
    while (!(UART1_ISR & UART_ISR_RXNE) && timeout_cycles--) {
        // Pequeña pausa para no saturar el bus
        for (volatile int i = 0; i < 100; i++);
    }
    
    if (timeout_cycles == 0) return false;  // Timeout
    
    // Leer byte recibido
    *byte = (uint8_t)(UART1_RDR & 0xFF);
    
    return true;
}

bool uart_hardware_send_string(const char* str) {
    if (!str) return false;
    
    while (*str) {
        if (!uart_hardware_send_byte(*str)) {
            return false;
        }
        str++;
    }
    
    return true;
}

bool uart_hardware_receive_string(char* buffer, int max_len, uint32_t timeout_ms) {
    if (!buffer || max_len <= 0) return false;
    
    int index = 0;
    uint8_t byte;
    uint32_t start_time = 0;  // En implementación real, usar timer del sistema
    
    // Leer hasta encontrar '\n' o timeout
    while (index < max_len - 1) {
        if (uart_hardware_receive_byte(&byte, 100)) {  // 100ms por byte
            if (byte == '\n') {
                buffer[index] = '\0';
                return true;
            }
            buffer[index++] = byte;
        } else {
            // Timeout en byte individual, verificar timeout total
            // En implementación real, verificar tiempo transcurrido
            break;
        }
    }
    
    buffer[index] = '\0';
    return index > 0;
}

// Función de prueba de loopback
bool uart_hardware_test_loopback(void) {
    const char test_msg[] = "TEST";
    char received[10];
    
    // Enviar mensaje de prueba
    if (!uart_hardware_send_string(test_msg)) {
        return false;
    }
    
    // Intentar recibir (solo funciona si TX está conectado a RX)
    if (uart_hardware_receive_string(received, sizeof(received), 1000)) {
        return (strcmp(received, test_msg) == 0);
    }
    
    return false;
} 