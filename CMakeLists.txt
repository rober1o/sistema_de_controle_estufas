cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Detectar o sistema operacional para definir o caminho do SDK
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

# Definir versões do SDK e ferramentas
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

# Incluir o suporte do VS Code se o arquivo existir
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Definir o tipo de placa (altere para 'pico' ou 'pico_w' conforme necessário)
set(PICO_BOARD pico CACHE STRING "Board type")

# Incluir o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Definir o projeto
project(refrigerador C CXX ASM)

# Inicializar o SDK do Pico
pico_sdk_init()

# Adicionar executável e arquivos necessários
add_executable(refrigerador 
    refrigerador.c 
    lib/ssd1306.c  # Arquivo de controle do display OLED
)

# Definir nome e versão do programa
pico_set_program_name(refrigerador "refrigerador")
pico_set_program_version(refrigerador "0.1")

# Gerar cabeçalho PIO (se necessário)
pico_generate_pio_header(refrigerador ${CMAKE_CURRENT_LIST_DIR}/pio_wave.pio)

# Habilitar saída via UART/USB
pico_enable_stdio_uart(refrigerador 1)
pico_enable_stdio_usb(refrigerador 1)

# Vincular bibliotecas necessárias
target_link_libraries(refrigerador
    pico_stdlib
    hardware_pio   # Suporte para LEDs endereçáveis
    hardware_i2c   # Suporte para o display OLED via I2C
    hardware_pwm   # Suporte para PWM (Joystick analógico, motores, LEDs, etc.)
    hardware_adc   # Suporte para ADC (Leitura do joystick analógico)
)

# Incluir diretórios necessários
target_include_directories(refrigerador PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc  # Certifique-se de que "inc" contém os headers adequados
)

# Gerar arquivos de saída adicionais
pico_add_extra_outputs(refrigerador)
