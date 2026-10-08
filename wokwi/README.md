# Simulação Wokwi — Gym Pole Monitor

Esta pasta contém a simulação do sistema **Gym Pole Monitor** desenvolvida no Wokwi, utilizada para validar a lógica de funcionamento do protótipo antes da implementação física do dispositivo IoT.

## Objetivo

O protótipo tem como objetivo identificar automaticamente se uma estação de polia está disponível ou ocupada, além de permitir que um funcionário coloque a estação em modo de manutenção.

A simulação reproduz o funcionamento do sistema utilizando um ESP32, sensor ultrassônico, LEDs indicadores e um botão para representar a ação de manutenção que futuramente será realizada pelo aplicativo.

## Componentes utilizados

* ESP32
* Sensor ultrassônico HC-SR04
* LED RGB
* LED vermelho
* Push button
* Resistores de 220 Ω
* Resistores de 10 kΩ

## Funcionamento

O sistema possui três estados principais:

* **Verde — Disponível:** nenhuma pessoa é detectada na estação.
* **Vermelho — Ocupada:** o sensor detecta uma pessoa durante um período de confirmação.
* **Azul — Manutenção:** a estação foi colocada manualmente em manutenção.

O modo de manutenção possui prioridade sobre a leitura do sensor.

### Confirmação de ocupação

Para evitar alterações rápidas de estado causadas por movimentos durante a utilização do equipamento, o sistema não considera uma única leitura do sensor suficiente para declarar a estação ocupada.

A ocupação é confirmada após aproximadamente **2 segundos de detecção contínua**.

Da mesma forma, quando uma pessoa deixa a área de detecção, o sistema aguarda aproximadamente **5 segundos sem detecção** antes de liberar a estação.

Esse mecanismo reduz oscilações entre os estados:

`Disponível → Ocupada → Disponível`

causadas por pequenas variações nas leituras do sensor.

## Lógica do sistema

```text
                    ESP32
                      │
                      ▼
                 HC-SR04
                      │
              Detecta presença?
                 /          \
               NÃO          SIM
                │             │
                ▼             ▼
           🟢 DISPONÍVEL   Confirmação
                              │
                           2 segundos
                              │
                              ▼
                         🔴 OCUPADA


              Botão de manutenção
                       │
                       ▼
                 🔵 MANUTENÇÃO
                       │
                       ▼
              Prioridade sobre sensor
```

## Simulação

O Wokwi é utilizado nesta etapa para validar a lógica do ESP32 e dos componentes eletrônicos antes da evolução para o hardware físico definitivo.

O botão presente na simulação representa a futura ação de manutenção realizada pelo aplicativo de gerenciamento da academia.

## Estrutura

```text
wokwi/
├── README.md
├── projeto/
│   └── arquivos da simulação Wokwi
└── imagens/
    └── prototipo-final.png
```

## Próximas etapas

* Validar o comportamento do protótipo físico com ESP32.
* Avaliar sensores mais adequados para detecção de presença em diferentes exercícios.
* Implementar comunicação Wi-Fi.
* Desenvolver a API de comunicação entre o ESP32 e o aplicativo.
* Integrar o aplicativo React Native ao sistema IoT.
* Desenvolver o gerenciamento de múltiplas estações.

