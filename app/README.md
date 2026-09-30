# Aplicativo — Gym Pole Monitor

Aplicativo desenvolvido para permitir que funcionários de academias monitorem e gerenciem o estado das estações de polia.

A aplicação será desenvolvida utilizando **React Native**.

## Objetivo

O aplicativo permitirá que funcionários autorizados visualizem as estações de uma academia e alterem manualmente o estado de uma estação quando ela estiver em manutenção ou indisponível.

## Estados da estação

A estação poderá apresentar os seguintes estados:

* 🟢 **Disponível** — estação livre para utilização.
* 🔴 **Ocupada** — estação em utilização, identificada pelo sensor.
* 🔵 **Manutenção** — estação temporariamente indisponível, definida por um funcionário.

## Fluxo planejado

```text
Funcionário
     ↓
Aplicativo React Native
     ↓
Backend / API
     ↓
Estação IoT
     ↓
LED indicador
```

## Desenvolvimento

O aplicativo será desenvolvido inicialmente de forma independente do hardware.

A integração com o sistema IoT será implementada posteriormente através de uma API, permitindo que o aplicativo altere o estado das estações remotamente.

## Tecnologias planejadas

* React Native
* JavaScript ou TypeScript
* API REST
* Backend
* Banco de dados

## Roadmap

* [ ] Criar projeto React Native
* [ ] Criar tela inicial
* [ ] Criar representação das estações
* [ ] Implementar estados: disponível, ocupada e manutenção
* [ ] Criar tela de detalhes da estação
* [ ] Criar ação para colocar estação em manutenção
* [ ] Criar ação para liberar estação
* [ ] Implementar autenticação de funcionários
* [ ] Criar API
* [ ] Integrar aplicativo com backend
* [ ] Integrar backend com dispositivo IoT
* [ ] Testar comunicação completa

