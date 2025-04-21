#include "refrigerador.h"



int main()
{
    
    add_repeating_timer_ms(1000, callback_timer, NULL, &timer);

    inicializar_hardware();

    desenha_fig(matriz_apagada, BRILHO_PADRAO, pio, sm);

    while (true)
    {
        adc_select_input(0);
        uint16_t leitura_estufa_A = adc_read();
        adc_select_input(1);
        uint16_t leitura_estufa_B = adc_read();

        // Converte valores dos sensores para porcentagem ajustando de acordo o erro do joystick
        temperatura_estufa_B = (abs(leitura_estufa_B - 2047) * 100) / 2047 - (MARGEM_ERRO/100);
        temperatura_estufa_A = (abs(leitura_estufa_A - 2047) * 100) / 2047 - (MARGEM_ERRO/100);

        if (temperatura_estufa_A > (numero * 10))
        {
            printf("ALERTA: REFRIGERADOR AZUL COM TEMPERATURA ALTA\n");
            bip_alerta();
        }else if(temperatura_estufa_B > (numero * 10)){
            printf("ALERTA: REFRIGERADOR VERMELHO COM TEMPERATURA ALTA\n");
            bip_alerta();
        }

        pwm_set_gpio_level(LED_BLUE, deslocamento_y(leitura_estufa_A));
        pwm_set_gpio_level(LED_RED, deslocamento_x(leitura_estufa_B));

        int pos_x = centro_x + ((2048 - (int)leitura_estufa_B) * centro_x) / 2048;
        int pos_y = centro_y + ((2048 - (int)leitura_estufa_A) * centro_y) / 2048;

        atualizar_display();
        ssd1306_rect(&display_oled, pos_y, pos_x, quadrado, quadrado, true, true);
        ssd1306_send_data(&display_oled);

        if (botao_pressionado)
        {
            printf("A temperatura máxima foi alterada para %d ºC\n", numero * 10);
            botao_pressionado = false;
        }

    }
}

void inicializar_hardware()
{
    inicializar_pwm(LED_BLUE, PWM_LIMITE, PWM_DIVISOR);
    inicializar_pwm(LED_RED, PWM_LIMITE, PWM_DIVISOR);

    adc_init();
    adc_gpio_init(SENSOR_REFRI_AZUL);
    adc_gpio_init(SENSOR_REFRI_VERMELHO);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Iniciar buzzer
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_pull_down(BUZZER_PIN);

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &callback_botoes);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED
    ssd1306_init(&display_oled, 128, 64, false, ENDERECO_DISPLAY, I2C_PORT);
    centro_x = (WIDTH - quadrado) / 2;
    centro_y = (HEIGHT - quadrado) / 2;

    // Define o PIO 0 para controle da matriz de LEDs
    pio = pio0;

    // Configura o clock do sistema para 133 MHz
    bool clock_setado = set_sys_clock_khz(133000, false);

    // Inicializa a comunicação serial
    stdio_init_all();

    // Exibe mensagem na serial caso o clock tenha sido configurado com sucesso
    if (clock_setado)
        printf("Clock setado %ld\n", clock_get_hz(clk_sys));

    // Carrega o programa PIO para controle da matriz de LEDs
    int offset = pio_add_program(pio, &Matriz_5x5_program);

    // Obtém um state machine livre para o PIO
    sm = pio_claim_unused_sm(pio, true);

    // Inicializa o programa PIO na matriz de LEDs
    Matriz_5x5_program_init(pio, sm, offset, MATRIZ_PIN);
}

// Função para verificar debounce
bool debounce_ok(uint gpio)
{
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (gpio == BUTTON_A)
    {
        if (tempo_atual - ultimo_tempoA > DEBOUNCE_TIME)
        {
            ultimo_tempoA = tempo_atual;
            return true;
        }
    }
    else if (gpio == BUTTON_B)
    {
        if (tempo_atual - ultimo_tempoB > DEBOUNCE_TIME)
        {
            ultimo_tempoB = tempo_atual;
            return true;
        }
    }
    return false;
}

// Função para inicializar o PWM
void inicializar_pwm(uint pino, uint limite, float divisor)
{
    gpio_set_function(pino, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pino);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, limite);
    pwm_config_set_clkdiv(&config, divisor);
    pwm_init(slice, &config, true);
}

// Mapeamento do eixo Y com zona morta
int16_t deslocamento_y(uint16_t valor)
{
    int16_t deslocamento = (valor < 2047) ? (2047 - valor) : (valor - 2047);
    return (deslocamento < MARGEM_ERRO) ? 0 : deslocamento;
}

// Mapeamento do eixo X com zona morta
int16_t deslocamento_x(uint16_t valor)
{
    int16_t deslocamento = (valor < 2047) ? (2047 - valor) : (valor - 2047);
    return (deslocamento < MARGEM_ERRO) ? 0 : deslocamento;
}

// Desenha bordas no display
void atualizar_display()
{
    ssd1306_config(&display_oled);
    ssd1306_fill(&display_oled, false);
    ssd1306_rect(&display_oled, 3, 3, 122, 60, true, false);
}

// Callback dos botões
void callback_botoes(uint pino, uint32_t eventos)
{
    if ((eventos & GPIO_IRQ_EDGE_FALL) && debounce_ok(pino))
    { // Verifica se passou 200ms desde a última chamada do botão

        if (pino == BUTTON_A && numero < 9)
        {             // Verifica se a interrupçãoveio do botão A e se já não atingiu o valor máximo da matriz
            numero++; // Incrementa o número
        }
        else if (pino == BUTTON_B && numero > 0)
        {             // Verifica se a interrupção veio do botão B e se já não atingiu o valor mínimo da matriz
            numero--; // Decrementa o número
        }

        botao_pressionado = true; // acionará a flag que será usada no loop principal para emitir alerta sobre mudança do valor

        exibir_numero(); // Atualiza a exibição do número
    }
}

// Inicializa hardware e periféricos

void desenha_fig(uint32_t *_matriz, uint8_t _intensidade, PIO pio, uint sm)
{
    uint32_t pixel = 0;
    uint8_t r, g, b;

    for (int i = 24; i > 19; i--) // Linha 1
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 15; i < 20; i++) // Linha 2
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (b << 16) | (r << 8) | g;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 14; i > 9; i--) // Linha 3
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 5; i < 10; i++) // Linha 4
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 4; i > -1; i--) // Linha 5
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }
}

// Rotina para exibir os números na matriz de LED
void exibir_numero()
{
    switch (numero)
    {
    case 0:
        desenha_fig(numero_0, BRILHO_PADRAO, pio, sm);
        break;
    case 1:
        desenha_fig(numero_1, BRILHO_PADRAO, pio, sm);
        break;
    case 2:
        desenha_fig(numero_2, BRILHO_PADRAO, pio, sm);
        break;
    case 3:
        desenha_fig(numero_3, BRILHO_PADRAO, pio, sm);
        break;
    case 4:
        desenha_fig(numero_4, BRILHO_PADRAO, pio, sm);
        break;
    case 5:
        desenha_fig(numero_5, BRILHO_PADRAO, pio, sm);
        break;
    case 6:
        desenha_fig(numero_6, BRILHO_PADRAO, pio, sm);
        break;
    case 7:
        desenha_fig(numero_7, BRILHO_PADRAO, pio, sm);
        break;
    case 8:
        desenha_fig(numero_8, BRILHO_PADRAO, pio, sm);
        break;
    case 9:
        desenha_fig(numero_9, BRILHO_PADRAO, pio, sm);
        break;
    default:
        break;
    }
}

void som_buz(uint16_t freq, uint16_t duration_ms)
{
    uint period = 1000000 / freq;                // Período do sinal em microssegundos
    uint cycles = (duration_ms * 1000) / period; // Número de ciclos a gerar

    for (uint i = 0; i < cycles; i++)
    {
        gpio_put(BUZZER_PIN, 1); // Liga o buzzer
        sleep_us(period / 2);    // Espera metade do período
        gpio_put(BUZZER_PIN, 0); // Desliga o buzzer
        sleep_us(period / 2);    // Espera metade do período
    }
}

void bip_alerta()
{
    for (int i = 0; i < 3; i++)
    {
        som_buz(1000, 300); // Bip a 1000 Hz por 150ms
        sleep_ms(150);      // Espera 150ms entre os bips
    }
}


bool callback_timer(repeating_timer_t *t)
{
    printf("ESTUFA A (AZUL): %dºC\n", temperatura_estufa_A);
    printf("ESTUFA B (VERMELHA): %dºC\n", temperatura_estufa_B);
    return true; 
}