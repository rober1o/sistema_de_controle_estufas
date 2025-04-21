# SISTEMA EMBARCADO PARA MONITORAMENTO DE TEMPERATURA DE ESTUFAS

Este projeto tem como objetivo desenvolver um sistema de alerta para monitoramento de temperatura de estufas, utilizando exclusivamente a placa BitDogLab.

A proposta consiste em permitir, por meio dos botões da placa, a definição da temperatura máxima permitida. Caso essa temperatura seja ultrapassada, o sistema emitirá um alerta sonoro.

Para simular as variações de temperatura dos dois refrigeradores, será utilizado o joystick da placa, que gera sinais analógicos. Esses sinais serão convertidos em dados digitais para análise. Com base nesses valores, o sistema controlará a intensidade de um LED via PWM, aumentando a luminosidade conforme a temperatura se eleva. Além disso, um alerta será acionado caso os valores ultrapassem o limite definido.
 

## Componentes Utilizados

1. **LED RGB** 
2. **Matriz de LED 5x5 WS2812** 
3. **Microcontrolador Raspberry Pi Pico W**
4. **Dois botões Pushbutton**
5. **Buzzer**
6 . **Joystick**
7. **Display OLED 1306**

## Funcionalidade

Ao iniciar o programa, o display OLED exibirá constantemente um quadrado que se movimenta conforme a movimentação do joystick.
Além disso, a cada segundo serão impressos no Serial Monitor os valores de temperatura das estufas A e B (Azul e vermelho respectivamente), em graus Celsius.

**MOVIMENTANDO O JOYSTICK EIXO Y**

Ao movimentar o joystick no eixo Y, a temperatura do refrigerador azul aumentará, variando de 0 até 100 graus, estando em 0 no centro e 100 nas extremidades.

Simultaneamente, o LED azul aumentará seu brilho proporcionalmente aos graus apresentados: Apagado em 0°C e  Totalmente aceso em 100°C

**MOVIMENTANDO O JOYSTICK EIXO X**

Ao movimentar o joystick no eixo X, a temperatura do refrigerador vermelho aumentará, variando de 0 até 100 graus, estando em 0 no centro e 100 nas extremidades.
Simultaneamente, o LED vermelho aumentará seu brilho proporcionalmente aos graus apresentados: Apagado em 0°C e Totalmente aceso em 100°C

**FUNCIONALIDADE DOS BOTÕES**

Botão A: Incrementa um número na matriz de LEDs. Esse número representa a temperatura máxima que o refrigerador pode atingir, sendo multiplicado por 10.
Exemplo: Se o valor for 2, o sistema suportará no máximo 20 graus.

Botão B: Decrementa um número na matriz de LEDs. Segue a mesma lógica do botão A: o valor representa a temperatura máxima, multiplicado por 10.
Exemplo: Se o valor for 2, o sistema suportará no máximo 20 graus.

Obs.: Caso algum dos refrigeradores ultrapasse o valor definido na matriz de LED, um alerta sonoro será emitido pelo buzzer, indicando superaquecimento no refrigerador.

### Como Usar

#### Usando o Simulador Wokwi

- Clone este repositório: git clone https://github.com/rober1o/sistema_refrigerador.git;
- Usando a extensão Raspberry Pi Pico importar o projeto;
- Compilar o código e clicar no arquivo diagram.json.

#### Usando a BitDogLab

- Clone este repositório: git clone https://github.com/rober1o/sistema_refrigerador.git;
- Usando a extensão Raspberry Pi Pico importar o projeto;
- Compilar o projeto;
- Plugar a BitDogLab usando um cabo apropriado e gravar o código.

## Atenção

O brilho dos LEDs é muito intenso, para evitar danos aos olhos recomendamos que o brilho_padrao seja ajustado para 10% quando estiver usando o kit BitDogLab.
 Caso necessário ajuste a variavel MARGEM_DE_ERRO para correção do Joystick.
## Demonstração

<!-- TODO: adicionar link do vídeo -->
Vídeo demonstrando as funcionalidades da solução implementada: [Demonstração]()
