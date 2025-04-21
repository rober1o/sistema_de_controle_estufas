#ifndef REFRIGERADOR  // Previne múltiplas inclusões do cabeçalho
#define REFRIGERADOR

// =============================
// Bibliotecas padrão e específicas do Raspberry Pi Pico
// =============================
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"   // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/pio.h"  // Controle do PIO (Programável I/O)
#include "hardware/clocks.h" // Manipulação de clock
#include "hardware/i2c.h"  // Comunicação I2C
#include "pico/bootrom.h"  // Funções de bootloader
#include "pico/time.h"     // Manipulação de tempo
#include "hardware/adc.h"
#include "pico/time.h"
#include "hardware/pwm.h"

// =============================
// Bibliotecas do projeto
// =============================
#include "pio_wave.pio.h"  // Código PIO 
#include "numeros.h"       // Arrays para exibir na matriz de LED
#include "lib/ssd1306.h"   // Controle do display OLED SSD1306
#include "lib/font.h"      // Manipulação de fontes

// =============================
// Definições de hardware
// =============================

// LEDs RGB
#define LED_BLUE 12
#define LED_RED 13

// buzzer
#define BUZZER_PIN  21 //Buzzer A da BitDog

// Botões
#define BUTTON_A 5
#define BUTTON_B 6

// Configurações gerais
#define BRILHO_PADRAO 20  // Intensidade padrão do brilho
#define DEBOUNCE_TIME 200
// Matriz de LEDs
#define MATRIZ_PIN 7       // Pino da matriz de LEDs
#define NUM_PIXELS 25      // Número de pixels na matriz

// Configurações I2C (Display OLED)
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO_DISPLAY 0x3C  // Endereço I2C do display OLED


// configurações do joystic e PWM
#define SENSOR_REFRI_AZUL 26
#define SENSOR_REFRI_VERMELHO 27
#define BOTAO_JOYSTICK 22
#define PWM_LIMITE 4095
#define PWM_DIVISOR 30.52f
#define MARGEM_ERRO 300   // Define a faixa onde os LEDs permanecerão apagados
#define BUZZER_PIN 21 // Defina o pino do buzzer


// =============================
// Variáveis globais
// =============================

volatile uint32_t ultimo_tempoA = 0;  // Última leitura do botão A
volatile uint32_t ultimo_tempoB = 0;  // Última leitura do botão B
volatile bool ligado = false;         // Estado do sistema
ssd1306_t display_oled;
volatile bool sistema_desligado = false;
bool sirene_ativa = true; // Variável de estado
const int quadrado = 8;
int numero = 1;
int centro_x;
int centro_y;
bool led_joystick = true;
absolute_time_t ultima_interrupcao = 0;
ssd1306_t display_oled;
bool botao_pressionado = false;
int percentual_refrigerador_vermelho = 0;
int percentual_refrigerador_azul = 0;
repeating_timer_t timer;
// Instâncias de hardware
ssd1306_t ssd;  // Display OLED
PIO pio;        // Instância do PIO
int sm;         // Máquina de estado do PIO


// =============================
// Declaração das funções
// =============================

void configurar_i2c(void);

void inicializar_hardware();
void inicializar_pwm(uint pino, uint limite, float divisor);
void desenha_fig(uint32_t *_matriz, uint8_t _intensidade, PIO pio, uint sm);
void configurar_display(ssd1306_t *ssd);
void exibir_numero();
void callback_botoes(uint pino, uint32_t eventos);
void atualizar_display();
int16_t deslocamento_x(uint16_t valor);
int16_t deslocamento_y(uint16_t valor);
void som_buz(uint16_t freq, uint16_t duration_ms);
bool callback_timer(repeating_timer_t *t);
void bip_alerta();
#endif // REFRIGERADOR
