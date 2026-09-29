# 🏋️ Gym Pole Monitor

> Sistema IoT desenvolvido com Arduino para monitoramento em tempo real da disponibilidade de equipamentos de academia, utilizando sensores ultrassônicos e indicadores luminosos.

## 📌 Sobre o projeto

Academias de grandes redes costumam apresentar alta demanda por determinados equipamentos, especialmente **estações de polia**, que frequentemente ficam ocupadas durante os horários de maior movimento.

Além da espera causada pela ocupação dos equipamentos, outro problema comum é a indisponibilidade temporária causada por **manutenção ou falhas**, como rompimento de cabos e problemas mecânicos.

O **Gym Pole Monitor** foi desenvolvido como uma proposta de solução IoT para permitir que os usuários identifiquem rapidamente o estado de uma estação de polia através de um sistema visual de sinalização.

O projeto utiliza um **sensor ultrassônico** para identificar a presença de um usuário na estação e LEDs para representar seu estado atual.

### Estados da estação

| LED         | Estado     | Significado                                 |
| ----------- | ---------- | ------------------------------------------- |
| 🟢 Verde    | Disponível | A estação está livre para utilização        |
| 🔴 Vermelho | Ocupada    | A estação está sendo utilizada              |
| 🔵 Azul     | Manutenção | A estação está indisponível para utilização |

A proposta é semelhante ao conceito utilizado em **sensores de estacionamento**, nos quais um sistema identifica a presença de um veículo e informa visualmente a disponibilidade da vaga.

---

## 🎯 Objetivo

Desenvolver um protótipo IoT capaz de monitorar automaticamente a disponibilidade de uma estação de polia em uma academia e informar seu estado através de indicadores luminosos.

O sistema busca reduzir o tempo gasto pelos usuários procurando equipamentos disponíveis e fornecer uma indicação visual simples e intuitiva sobre o estado da estação.

---

## 💡 Problema

Em academias movimentadas, o usuário normalmente precisa se deslocar pelo ambiente procurando uma estação disponível.

Isso pode gerar:

* Tempo perdido procurando equipamentos;
* Filas e aglomerações próximas às estações;
* Dificuldade para identificar rapidamente quais equipamentos estão livres;
* Incerteza sobre equipamentos temporariamente interditados;
* Necessidade de verificar visualmente se uma estação está disponível.

Além disso, uma estação que apresenta algum problema mecânico pode permanecer indisponível sem uma sinalização clara para os usuários.

---

## 🚀 Solução proposta

O Gym Pole Monitor utiliza um sensor ultrassônico instalado próximo à estação de polia para detectar a presença de uma pessoa utilizando o equipamento.

A partir da distância medida pelo sensor, o Arduino determina o estado da estação.

```text
                 ┌─────────────────┐
                 │ Sensor           │
                 │ Ultrassônico     │
                 └────────┬────────┘
                          │
                          │ Distância
                          ▼
                 ┌─────────────────┐
                 │     Arduino     │
                 │                 │
                 │ Processamento   │
                 │ da informação   │
                 └────────┬────────┘
                          │
              ┌───────────┼───────────┐
              │           │           │
              ▼           ▼           ▼
         ┌────────┐  ┌─────────┐  ┌────────┐
         │  LED   │  │   LED   │  │  LED   │
         │ Verde  │  │ Vermelho│  │  Azul  │
         └────────┘  └─────────┘  └────────┘
          Livre       Ocupada      Manutenção
```

---

## ⚙️ Funcionamento

O sistema possui três estados principais.

### 🟢 Estação disponível

Quando nenhuma pessoa é detectada dentro da distância configurada pelo sistema:

**LED verde → ligado**

Isso indica que a estação está disponível para utilização.

---

### 🔴 Estação ocupada

Quando o sensor ultrassônico identifica a presença de uma pessoa dentro da área de utilização:

**LED vermelho → ligado**

Isso indica que a estação está atualmente ocupada.

---

### 🔵 Estação em manutenção

Quando a estação apresenta algum problema ou precisa ser temporariamente retirada de operação:

**LED azul → ligado**

Nesse estado, a estação é considerada indisponível independentemente da leitura do sensor.

No protótipo inicial, o modo de manutenção poderá ser acionado manualmente através de um botão ou chave.

Em uma futura versão, esse estado poderá ser controlado remotamente através de uma aplicação ou dashboard.

---

## 🧠 Lógica do sistema

A lógica inicial pode ser representada da seguinte forma:

```text
                  INÍCIO
                     │
                     ▼
            Ler estado do sistema
                     │
                     ▼
          Modo manutenção ativo?
                 /       \
               SIM       NÃO
                │         │
                ▼         ▼
           LED AZUL   Ler distância
                          │
                          ▼
                Pessoa detectada?
                    /        \
                  SIM        NÃO
                   │          │
                   ▼          ▼
              LED VERMELHO  LED VERDE
                 OCUPADA     DISPONÍVEL
```

A prioridade do sistema será:

```text
MANUTENÇÃO
    ↓
OCUPADA
    ↓
DISPONÍVEL
```

Ou seja, uma estação configurada como "em manutenção" continuará indicando o estado de manutenção mesmo que o sensor detecte uma pessoa.

---

## 🛠️ Componentes

O protótipo inicial será desenvolvido utilizando:

* Arduino Uno;
* Protoboard;
* Sensor ultrassônico HC-SR04;
* LED verde;
* LED vermelho;
* LED azul;
* Resistores;
* Jumpers;
* Botão/chave para controle do modo de manutenção.

Componentes adicionais poderão ser incorporados conforme o projeto evoluir.

---

## 💻 Tecnologias e ferramentas

### Hardware

* Arduino Uno
* HC-SR04
* LEDs
* Protoboard
* Resistores
* Jumpers
* Push button / switch

### Software

* Arduino IDE
* Tinkercad Circuits
* C/C++
* Git
* GitHub

---

## 🧪 Simulação

O primeiro estágio do projeto será desenvolvido no **Tinkercad Circuits**.

A simulação permitirá validar:

* Ligações dos componentes;
* Leitura do sensor ultrassônico;
* Distância de detecção;
* Acionamento dos LEDs;
* Lógica de ocupação;
* Lógica de disponibilidade;
* Modo de manutenção;
* Comportamento do sistema em diferentes situações.

### Circuito no Tinkercad

O circuito será disponibilizado aqui após a criação da primeira versão:

**[Tinkercad — Gym Pole Monitor](ADICIONAR_LINK_AQUI)**

As imagens das simulações serão armazenadas em:

```text
tinkercad/imagens/
```

---

## 🔌 Implementação com Arduino

Após a validação da lógica no Tinkercad, o projeto será implementado fisicamente utilizando Arduino e os componentes eletrônicos do protótipo.

O firmware será desenvolvido utilizando a Arduino IDE.

O código principal está localizado em:

```text
arduino/src/gym_pole_monitor/gym_pole_monitor.ino
```

A implementação física permitirá comparar o comportamento do protótipo com os resultados obtidos na simulação.

---

## 📊 Evolução planejada

O projeto será desenvolvido de maneira incremental.

### Fase 1 — Protótipo

* [x] Definição do problema
* [x] Definição da solução
* [x] Definição dos estados da estação
* [ ] Montagem do circuito no Tinkercad
* [ ] Programação inicial do Arduino
* [ ] Testes da detecção de presença
* [ ] Implementação dos três estados

### Fase 2 — Hardware

* [ ] Montagem na protoboard
* [ ] Testes com Arduino físico
* [ ] Ajuste da distância de detecção
* [ ] Testes de estabilidade
* [ ] Simulação de diferentes cenários

### Fase 3 — IoT

Uma futura versão poderá transformar o protótipo em uma solução IoT conectada.

Possibilidades:

* Monitoramento remoto das estações;
* Dashboard de disponibilidade;
* Histórico de utilização;
* Tempo médio de ocupação;
* Quantidade de utilizações por período;
* Identificação de equipamentos frequentemente indisponíveis;
* Comunicação entre múltiplas estações;
* Notificação de manutenção;
* Monitoramento centralizado de toda a academia.

Uma arquitetura futura poderia ser:

```text
┌───────────────┐
│    Polia 01   │
│   Arduino +   │
│    Sensor     │
└───────┬───────┘
        │
        │
┌───────▼───────┐
│   Internet /  │
│   Gateway IoT │
└───────┬───────┘
        │
        ▼
┌────────────────┐
│     Backend    │
│                │
│ Dados + API    │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│    Dashboard   │
│                │
│ 🟢 🟢 🔴 🔵   │
│ 🟢 🔴 🟢 🟢   │
└────────────────┘
```

---

## 📈 Dados

A pasta `data/` será utilizada futuramente para armazenar dados coletados pelo sistema.

Entre as informações que poderão ser analisadas:

* Horário de utilização;
* Tempo de ocupação;
* Tempo disponível;
* Frequência de utilização;
* Períodos de maior demanda;
* Quantidade de ocorrências de manutenção;
* Tempo de indisponibilidade.

Esses dados poderão posteriormente ser utilizados para análises de **Data Science e Machine Learning**, como previsão de demanda e identificação de horários de maior utilização.

---

## 📂 Estrutura do projeto

```text
gym-pole-monitor/
│
├── README.md
├── .gitignore
│
├── docs/
│   ├── arquitetura.md
│   ├── componentes.md
│   └── funcionamento.md
│
├── tinkercad/
│   ├── README.md
│   ├── circuitos/
│   │   └── README.md
│   └── imagens/
│       └── .gitkeep
│
├── arduino/
│   ├── README.md
│   └── src/
│       └── gym_pole_monitor/
│           └── gym_pole_monitor.ino
│
├── images/
│   ├── arquitetura/
│   ├── tinkercad/
│   └── resultados/
│
└── data/
    └── README.md
```

---

## 🔮 Possíveis melhorias

O protótipo inicial será propositalmente simples, mas a arquitetura permite diversas evoluções.

### Sensoriamento

* Utilização de sensores adicionais;
* Detecção mais precisa da presença;
* Monitoramento de posição;
* Detecção de anomalias.

### Conectividade

O Arduino poderá futuramente ser substituído ou complementado por uma plataforma com conectividade, como:

* ESP8266;
* ESP32;
* Wi-Fi;
* Bluetooth;
* MQTT.

### Software

Uma versão avançada poderá contar com:

* API;
* Banco de dados;
* Dashboard web;
* Aplicativo;
* Sistema de gerenciamento das estações.

### Data Science

Com dados suficientes, o projeto poderá evoluir para análises como:

* Previsão de horários de maior demanda;
* Identificação de equipamentos mais utilizados;
* Análise de disponibilidade;
* Previsão de necessidade de manutenção;
* Otimização da distribuição dos equipamentos.

---

## 📚 Documentação

| Documento                                        | Descrição                   |
| ------------------------------------------------ | --------------------------- |
| [`docs/arquitetura.md`](docs/arquitetura.md)     | Arquitetura do sistema      |
| [`docs/componentes.md`](docs/componentes.md)     | Componentes utilizados      |
| [`docs/funcionamento.md`](docs/funcionamento.md) | Funcionamento e lógica      |
| [`tinkercad/README.md`](tinkercad/README.md)     | Simulações                  |
| [`arduino/README.md`](arduino/README.md)         | Desenvolvimento do firmware |

---

## 👨‍💻 Autor

**Gabriel Guimarães de Oliveira**

Projeto desenvolvido como estudo prático de **IoT, sistemas embarcados, programação e automação**, explorando a integração entre hardware, software e análise de dados.

---

## 📄 Licença

Este projeto está sob a licença MIT.
