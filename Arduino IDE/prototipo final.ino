
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// 1. CONFIGURACOES
// =====================================================

const char* POLE_ID = "pole-01";

// Dois roteadores cadastrados
const char* WIFI_1_SSID = "ROTEADOR 01";
const char* WIFI_1_PASS = "SENHA";

const char* WIFI_2_SSID = "ROTEADOR 02";
const char* WIFI_2_PASS = "SENHA";

// Se o outro roteador estiver pelo menos 8 dBm mais forte,
// o ESP32 considera trocar de rede.
const int DIFERENCA_SINAL_TROCA = 8;

const unsigned long INTERVALO_VERIFICACAO_ROTEADORES = 15000;

unsigned long ultimaVerificacaoRoteadores = 0;
bool primeiraBuscaRoteadores = true;

// Controle de tentativas e mensagens Wi-Fi
unsigned long ultimaMensagemWiFi = 0;

// Rede local do ESP32
const char* AP_SSID = "GYM_POLE_MONITOR";
const char* AP_PASS = "GymPole123";

// Firebase Realtime Database
const char* FIREBASE_COMMAND_URL =
  "https://gym-pole-ui-default-rtdb.firebaseio.com/led.json";

const char* FIREBASE_STATUS_URL =
  "https://gym-pole-ui-default-rtdb.firebaseio.com/polias/pole-01/status.json";

// =====================================================
// 2. PINOS
// =====================================================

// LED RGB comum catodo
const int LED_R = 19;
const int LED_G = 22;
const int LED_B = 18;

// Botao: GPIO 23 ligado ao 3V3
const int BOTAO_PIN = 23;

// HC-SR04
const int TRIG_PIN = 15;
const int ECHO_PIN = 21;

// OLED SSD1306 I2C
const int OLED_SDA = 25;
const int OLED_SCL = 26;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

WebServer server(80);

// =====================================================
// 3. ESTADOS
// =====================================================

// 0 = automatico
// 1 = vermelho manual
// 2 = manutencao azul

volatile int comandoRemoto = 0;
volatile bool manutencaoLocal = false;
volatile bool pessoaDetectada = false;

float distanciaCm = -1.0;

enum EstadoPolia {
  ESTADO_VERDE,
  ESTADO_VERMELHO,
  ESTADO_AZUL
};

EstadoPolia ultimoEstadoVisual = ESTADO_VERDE;

// =====================================================
// 4. BOTAO COM DEBOUNCE
// =====================================================

const unsigned long DEBOUNCE_MS = 30;

int ultimaLeituraBotao = LOW;
int estadoEstavelBotao = LOW;

unsigned long ultimoDebounce = 0;
volatile bool eventoBotaoPendente = false;

// =====================================================
// 5. SENSOR
// =====================================================

const float DISTANCIA_DETECCAO_CM = 80.0;

const unsigned long INTERVALO_SENSOR_MS = 100;
const unsigned long TEMPO_LIBERAR_MS = 3000;

unsigned long ultimaLeituraSensor = 0;
unsigned long ultimaDeteccao = 0;

// =====================================================
// 6. FIREBASE E SINCRONIZACAO
// =====================================================

volatile int comandoFirebasePendente = -1;
volatile bool ignorarComandoRemotoAteSincronizar = false;

portMUX_TYPE firebaseMux = portMUX_INITIALIZER_UNLOCKED;

const unsigned long INTERVALO_STATUS_MS = 5000;
const unsigned long INTERVALO_LOG_WIFI_MS = 5000;

// =====================================================
// 7. FUNCOES AUXILIARES
// =====================================================

String nomeEstado(EstadoPolia estado) {
  switch (estado) {
    case ESTADO_VERDE:
      return "green";

    case ESTADO_VERMELHO:
      return "red";

    case ESTADO_AZUL:
      return "blue";
  }

  return "green";
}

String nomeModo() {
  if (manutencaoLocal) {
    return "manutencao_local";
  }

  if (comandoRemoto == 2) {
    return "manutencao_remota";
  }

  if (comandoRemoto == 1) {
    return "vermelho_manual";
  }

  return "automatico";
}

String nomeOrigem() {
  if (manutencaoLocal) {
    return "botao";
  }

  if (comandoRemoto == 2 || comandoRemoto == 1) {
    return "servidor";
  }

  return "sensor";
}

// =====================================================
// 8. CALCULO DO ESTADO DA POLIA
// =====================================================

EstadoPolia calcularEstado() {
  if (manutencaoLocal || comandoRemoto == 2) {
    return ESTADO_AZUL;
  }

  if (comandoRemoto == 1) {
    return ESTADO_VERMELHO;
  }

  if (pessoaDetectada) {
    return ESTADO_VERMELHO;
  }

  return ESTADO_VERDE;
}

// =====================================================
// 9. CONTROLE DO LED RGB
// =====================================================

void atualizarLED(EstadoPolia estado) {
  switch (estado) {
    case ESTADO_VERDE:
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, LOW);
      break;

    case ESTADO_VERMELHO:
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, LOW);
      break;

    case ESTADO_AZUL:
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, HIGH);
      break;
  }
}

// =====================================================
// 10. OLED
// =====================================================

unsigned long ultimaAtualizacaoOLED = 0;

void atualizarOLED(bool forcar = false) {
  if (!forcar &&
      millis() - ultimaAtualizacaoOLED < 200) {
    return;
  }

  ultimaAtualizacaoOLED = millis();

  EstadoPolia estado = calcularEstado();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(13, 2);
  display.println("GYM POLE MONITOR");

  display.drawLine(8, 14, 119, 14, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("ESTACAO ");
  display.println(POLE_ID);

  display.drawRoundRect(4, 32, 120, 29, 5, SSD1306_WHITE);

  display.setTextSize(2);

  const char* textoEstado;

  switch (estado) {
    case ESTADO_VERDE:
      textoEstado = "LIVRE";
      break;

    case ESTADO_VERMELHO:
      textoEstado = "OCUPADO";
      break;

    case ESTADO_AZUL:
      textoEstado = "MANUTENCAO";
      break;

    default:
      textoEstado = "LIVRE";
      break;
  }

  int16_t x1, y1;
  uint16_t largura, altura;

  display.getTextBounds(
    textoEstado, 0, 0,
    &x1, &y1, &largura, &altura
  );

  if (estado == ESTADO_AZUL) {
    display.setTextSize(1);

    display.getTextBounds(
      textoEstado, 0, 0,
      &x1, &y1, &largura, &altura
    );
  }

  int x = (SCREEN_WIDTH - largura) / 2;
  int y = (estado == ESTADO_AZUL) ? 43 : 41;

  display.setCursor(x, y);
  display.print(textoEstado);

  display.display();
}

// =====================================================
// 11. ATUALIZAR ESTADO VISUAL
// =====================================================

void atualizarEstadoVisual(bool forcarOLED = false) {
  EstadoPolia estado = calcularEstado();

  atualizarLED(estado);

  if (forcarOLED || estado != ultimoEstadoVisual) {
    atualizarOLED(true);
    ultimoEstadoVisual = estado;

    Serial.print("Estado da polia: ");
    Serial.println(nomeEstado(estado));
  } else {
    atualizarOLED(false);
  }
}

// =====================================================
// 12. FILA DE COMANDOS FIREBASE
// =====================================================

void enfileirarComandoFirebase(int comando) {
  portENTER_CRITICAL(&firebaseMux);

  comandoFirebasePendente = comando;
  ignorarComandoRemotoAteSincronizar = true;

  portEXIT_CRITICAL(&firebaseMux);
}

// =====================================================
// 13. BOTAO FISICO
// =====================================================

void verificarBotao() {
  int leitura = digitalRead(BOTAO_PIN);

  if (leitura != ultimaLeituraBotao) {
    ultimoDebounce = millis();
    ultimaLeituraBotao = leitura;
  }

  if (millis() - ultimoDebounce >= DEBOUNCE_MS) {
    if (leitura != estadoEstavelBotao) {
      estadoEstavelBotao = leitura;

      if (estadoEstavelBotao == HIGH) {
        eventoBotaoPendente = true;
      }
    }
  }
}

void processarEventoBotao() {
  if (!eventoBotaoPendente) {
    return;
  }

  eventoBotaoPendente = false;

  bool estadoManualAtivo =
    manutencaoLocal ||
    comandoRemoto == 1 ||
    comandoRemoto == 2;

  if (estadoManualAtivo) {
    manutencaoLocal = false;
    comandoRemoto = 0;

    enfileirarComandoFirebase(0);

    Serial.println(
      "BOTAO: atividade normal / modo automatico"
    );
  } else {
    manutencaoLocal = true;

    enfileirarComandoFirebase(2);

    Serial.println(
      "BOTAO: manutencao local ativada"
    );
  }

  atualizarEstadoVisual(true);
}

// =====================================================
// 14. SENSOR ULTRASSONICO
// =====================================================

void verificarSensor() {
  if (millis() - ultimaLeituraSensor <
      INTERVALO_SENSOR_MS) {
    return;
  }

  ultimaLeituraSensor = millis();

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duracao =
    pulseIn(ECHO_PIN, HIGH, 25000);

  if (duracao == 0) {
    distanciaCm = -1.0;
  } else {
    distanciaCm = duracao * 0.0343 / 2.0;
  }

  bool deteccaoAtual =
    distanciaCm > 0 &&
    distanciaCm <= DISTANCIA_DETECCAO_CM;

  if (deteccaoAtual) {
    pessoaDetectada = true;
    ultimaDeteccao = millis();
  }

  if (pessoaDetectada &&
      millis() - ultimaDeteccao >= TEMPO_LIBERAR_MS) {
    pessoaDetectada = false;
  }

  atualizarEstadoVisual();
}

// =====================================================
// 15. INTERPRETAR COMANDOS DO FIREBASE
// =====================================================

void processarComandoFirebase(String comando) {
  comando.trim();
  comando.replace("\"", "");
  comando.toLowerCase();

  portENTER_CRITICAL(&firebaseMux);
  bool ignorar = ignorarComandoRemotoAteSincronizar;
  portEXIT_CRITICAL(&firebaseMux);

  if (ignorar) {
    return;
  }

  if (comando == "blue" ||
      comando == "manutencao") {
    comandoRemoto = 2;
    manutencaoLocal = false;

    Serial.println("Firebase: manutencao azul");
  }
  else if (comando == "red" ||
           comando == "ocupado") {
    comandoRemoto = 1;
    manutencaoLocal = false;

    Serial.println("Firebase: vermelho manual");
  }
  else if (comando == "green" ||
           comando == "automatico" ||
           comando == "auto") {
    comandoRemoto = 0;
    manutencaoLocal = false;

    Serial.println("Firebase: modo automatico");
  }
  else {
    return;
  }
}

// =====================================================
// 16. ENVIAR COMANDO PARA O FIREBASE
// =====================================================

bool enviarComandoFirebase(int comando) {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  String valor;

  if (comando == 2) {
    valor = "blue";
  } else if (comando == 1) {
    valor = "red";
  } else {
    valor = "green";
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setConnectTimeout(800);
  http.setTimeout(800);

  if (!http.begin(client, FIREBASE_COMMAND_URL)) {
    return false;
  }

  http.addHeader("Content-Type", "application/json");

  String payload = "\"" + valor + "\"";

  int codigoHTTP = http.PUT(payload);
  http.end();

  if (codigoHTTP >= 200 && codigoHTTP < 300) {
    Serial.print("Comando sincronizado: ");
    Serial.println(valor);
    return true;
  }

  Serial.print("Falha ao enviar comando. HTTP: ");
  Serial.println(codigoHTTP);

  return false;
}

// =====================================================
// 17. CONSULTAR COMANDO DO FIREBASE
// =====================================================

void consultarFirebase() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setConnectTimeout(800);
  http.setTimeout(800);

  if (!http.begin(client, FIREBASE_COMMAND_URL)) {
    return;
  }

  int codigoHTTP = http.GET();

  if (codigoHTTP == HTTP_CODE_OK) {
    String resposta = http.getString();
    resposta.trim();
    resposta.replace("\"", "");
    resposta.toLowerCase();

    processarComandoFirebase(resposta);
  } else if (codigoHTTP > 0) {
    Serial.print("Falha ao consultar comando. HTTP: ");
    Serial.println(codigoHTTP);
  }

  http.end();
}

// =====================================================
// 18. PUBLICAR STATUS NO FIREBASE
// =====================================================

void publicarStatusFirebase() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  EstadoPolia estado = calcularEstado();

  String distanciaJSON;

  if (distanciaCm >= 0) {
    distanciaJSON = String(distanciaCm, 1);
  } else {
    distanciaJSON = "null";
  }

  String payload = "{";
  payload += "\"id\":\"" + String(POLE_ID) + "\",";
  payload += "\"status\":\"" + nomeEstado(estado) + "\",";
  payload += "\"modo\":\"" + nomeModo() + "\",";
  payload += "\"origem\":\"" + nomeOrigem() + "\",";
  payload += "\"distancia_cm\":" + distanciaJSON + ",";
  payload += "\"online\":true";
  payload += "}";

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setConnectTimeout(800);
  http.setTimeout(800);

  if (!http.begin(client, FIREBASE_STATUS_URL)) {
    return;
  }

  http.addHeader("Content-Type", "application/json");

  int codigoHTTP = http.PUT(payload);
  http.end();

  if (codigoHTTP >= 200 && codigoHTTP < 300) {
    Serial.println("Status da polia publicado no Firebase.");
  } else {
    Serial.print("Falha ao publicar status. HTTP: ");
    Serial.println(codigoHTTP);
  }
}

// =====================================================
// 19. SELECAO AUTOMATICA DO MELHOR ROTEADOR
// =====================================================

void selecionarMelhorRoteador() {
  unsigned long agora = millis();

  if (!primeiraBuscaRoteadores &&
      agora - ultimaVerificacaoRoteadores <
      INTERVALO_VERIFICACAO_ROTEADORES) {
    return;
  }

  primeiraBuscaRoteadores = false;
  ultimaVerificacaoRoteadores = agora;

  Serial.println();
  Serial.println("Procurando os roteadores configurados...");

  // Busca redes visiveis sem desligar o ponto de acesso do ESP32.
  int quantidade = WiFi.scanNetworks(false, true);

  if (quantidade < 0) {
    Serial.println("Falha na busca de redes Wi-Fi.");
    WiFi.scanDelete();
    return;
  }

  String melhorSSID = "";
  int melhorRSSI = -1000;
  int sinalAtual = -1000;

  String ssidAtual = "";

  if (WiFi.status() == WL_CONNECTED) {
    ssidAtual = WiFi.SSID();
  }

  for (int i = 0; i < quantidade; i++) {
    String ssidEncontrado = WiFi.SSID(i);
    int rssi = WiFi.RSSI(i);

    bool configurado =
      ssidEncontrado == WIFI_1_SSID ||
      ssidEncontrado == WIFI_2_SSID;

    if (!configurado) {
      continue;
    }

    Serial.print("Encontrado: ");
    Serial.print(ssidEncontrado);
    Serial.print(" | Sinal: ");
    Serial.print(rssi);
    Serial.println(" dBm");

    if (ssidEncontrado == ssidAtual) {
      sinalAtual = rssi;
    }

    if (rssi > melhorRSSI) {
      melhorRSSI = rssi;
      melhorSSID = ssidEncontrado;
    }
  }

  WiFi.scanDelete();

  if (melhorSSID == "") {
    Serial.println(
      "Nenhum dos dois roteadores foi encontrado."
    );

    // Se estiver desconectado, tenta novamente na proxima busca.
    return;
  }

  // Se estiver desconectado, conecta ao melhor encontrado.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Conectando ao roteador: ");
    Serial.println(melhorSSID);

    if (melhorSSID == WIFI_1_SSID) {
      WiFi.begin(WIFI_1_SSID, WIFI_1_PASS);
    } else {
      WiFi.begin(WIFI_2_SSID, WIFI_2_PASS);
    }

    return;
  }

  // Ja conectado ao melhor roteador: nao faz nada.
  if (ssidAtual == melhorSSID) {
    Serial.print("Mantendo conexao com: ");
    Serial.println(ssidAtual);
    return;
  }

  // Se o sinal atual nao apareceu na busca, considera
  // a possibilidade de ele estar fraco demais.
  if (sinalAtual != -1000) {
    int diferenca = melhorRSSI - sinalAtual;

    Serial.print("Diferenca de sinal: ");
    Serial.print(diferenca);
    Serial.println(" dBm");

    if (diferenca < DIFERENCA_SINAL_TROCA) {
      Serial.println(
        "Sinal alternativo nao e forte o suficiente. Mantendo rede atual."
      );
      return;
    }
  }

  Serial.print("Trocando de ");
  Serial.print(ssidAtual);
  Serial.print(" para ");
  Serial.println(melhorSSID);

  // Mantem o modo AP+STA; o painel local continua disponivel.
  WiFi.disconnect(false, false);
  delay(100);

  if (melhorSSID == WIFI_1_SSID) {
    WiFi.begin(WIFI_1_SSID, WIFI_1_PASS);
  } else {
    WiFi.begin(WIFI_2_SSID, WIFI_2_PASS);
  }
}

// =====================================================
// 20. TAREFA DE REDE SEPARADA
// =====================================================

void tarefaRede(void* parametro) {
  unsigned long ultimaConsulta = 0;
  unsigned long ultimaPublicacao = 0;
  unsigned long ultimaTentativaWiFi = 0;

  for (;;) {
    unsigned long agora = millis();

    if (agora - ultimaTentativaWiFi >= 1000) {
      ultimaTentativaWiFi = agora;

      selecionarMelhorRoteador();

      if (WiFi.status() == WL_CONNECTED &&
          agora - ultimaMensagemWiFi >= INTERVALO_LOG_WIFI_MS) {
        ultimaMensagemWiFi = agora;

        Serial.print("Wi-Fi conectado: ");
        Serial.println(WiFi.SSID());

        Serial.print("Intensidade do sinal: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");

        Serial.print("IP local da rede: ");
        Serial.println(WiFi.localIP());
      }
    }

    if (WiFi.status() == WL_CONNECTED) {
      int comandoPendente = -1;

      portENTER_CRITICAL(&firebaseMux);
      comandoPendente = comandoFirebasePendente;

      if (comandoPendente != -1) {
        comandoFirebasePendente = -1;
      }

      portEXIT_CRITICAL(&firebaseMux);

      if (comandoPendente != -1) {
        bool sucesso = enviarComandoFirebase(comandoPendente);

        portENTER_CRITICAL(&firebaseMux);

        if (sucesso) {
          if (comandoFirebasePendente == -1) {
            ignorarComandoRemotoAteSincronizar = false;
          }
        } else {
          if (comandoFirebasePendente == -1) {
            comandoFirebasePendente = comandoPendente;
          }
        }

        portEXIT_CRITICAL(&firebaseMux);
      }

      agora = millis();

      if (agora - ultimaConsulta >= 1500) {
        ultimaConsulta = agora;
        consultarFirebase();
      }

      agora = millis();

      if (agora - ultimaPublicacao >= INTERVALO_STATUS_MS) {
        ultimaPublicacao = agora;
        publicarStatusFirebase();
      }
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// =====================================================
// 21. PAINEL WEB LOCAL
// =====================================================

String paginaHTML() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Gym Pole Monitor</title>
<style>
body {
  font-family: Arial, sans-serif;
  background: #101820;
  color: white;
  text-align: center;
  margin: 0;
  padding: 24px 12px;
}
.card {
  max-width: 440px;
  margin: auto;
  padding: 20px;
  border-radius: 16px;
  background: #1c2935;
}
button {
  display: block;
  width: 100%;
  padding: 15px;
  margin: 12px 0;
  border: 0;
  border-radius: 10px;
  font-size: 17px;
  font-weight: bold;
  cursor: pointer;
}
.green { background: #25a65a; color: white; }
.red { background: #d73535; color: white; }
.blue { background: #2379db; color: white; }
.small { color: #b9c4ce; font-size: 14px; }
</style>
</head>
<body>
<div class="card">
  <h1>Gym Pole Monitor</h1>
  <p>Polia: pole-01</p>
  <h2 id="estado">Carregando...</h2>
  <p id="modo" class="small">Consultando estado...</p>
  <p id="distancia" class="small"></p>
  <button class="green" onclick="comando('green')">
    Verde - Voltar a atividade
  </button>
  <button class="red" onclick="comando('red')">
    Vermelho - Ocupado manualmente
  </button>
  <button class="blue" onclick="comando('blue')">
    Azul - Manutencao
  </button>
  <p class="small" id="mensagem">
    Painel conectado diretamente ao ESP32.
  </p>
</div>
<script>
async function comando(estado) {
  const mensagem = document.getElementById('mensagem');
  mensagem.textContent = 'Enviando comando...';

  try {
    const resposta = await fetch('/comando?estado=' + estado);
    mensagem.textContent = await resposta.text();
    atualizar();
  } catch (e) {
    mensagem.textContent = 'Falha na comunicacao com o ESP32.';
  }
}

async function atualizar() {
  try {
    const r = await fetch('/api/status');
    const d = await r.json();

    document.getElementById('estado').textContent = d.status;
    document.getElementById('modo').textContent = 'Modo: ' + d.modo;
    document.getElementById('distancia').textContent =
      'Distancia: ' + d.distancia_cm + ' cm';
  } catch (e) {}
}

setInterval(atualizar, 1000);
atualizar();
</script>
</body>
</html>
)HTML";

  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", paginaHTML());
}

void handleStatusLocal() {
  EstadoPolia estado = calcularEstado();

  String distanciaJSON;

  if (distanciaCm >= 0) {
    distanciaJSON = String(distanciaCm, 1);
  } else {
    distanciaJSON = "null";
  }

  String json = "{";
  json += "\"id\":\"" + String(POLE_ID) + "\",";
  json += "\"status\":\"" + nomeEstado(estado) + "\",";
  json += "\"modo\":\"" + nomeModo() + "\",";
  json += "\"distancia_cm\":" + distanciaJSON;
  json += "}";

  server.send(200, "application/json", json);
}

void handleComandoLocal() {
  if (!server.hasArg("estado")) {
    server.send(400, "text/plain", "Comando ausente.");
    return;
  }

  String estado = server.arg("estado");
  estado.trim();
  estado.toLowerCase();

  if (estado == "green") {
    comandoRemoto = 0;
    manutencaoLocal = false;
    enfileirarComandoFirebase(0);
  }
  else if (estado == "red") {
    comandoRemoto = 1;
    manutencaoLocal = false;
    enfileirarComandoFirebase(1);
  }
  else if (estado == "blue") {
    comandoRemoto = 2;
    manutencaoLocal = false;
    enfileirarComandoFirebase(2);
  }
  else {
    server.send(400, "text/plain", "Comando invalido.");
    return;
  }

  atualizarEstadoVisual(true);

  server.send(
    200,
    "text/plain",
    "Comando aplicado localmente. Sincronizacao com Firebase pendente."
  );
}

// =====================================================
// 22. SETUP
// =====================================================

void setup() {
  Serial.begin(115200);

  // LED
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);

  // Botao
  pinMode(BOTAO_PIN, INPUT_PULLDOWN);

  // Sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(13, 2);
    display.println("GYM POLE MONITOR");
    display.drawLine(8, 14, 119, 14, SSD1306_WHITE);
    display.setCursor(15, 28);
    display.println("Inicializando...");
    display.display();
  } else {
    Serial.println("ERRO: OLED nao encontrada.");
  }

  // Wi-Fi: ponto de acesso + cliente
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(true);

  bool apIniciado = WiFi.softAP(AP_SSID, AP_PASS);

  if (apIniciado) {
    Serial.println("Ponto de acesso local iniciado.");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("Painel local: http://");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("ERRO ao iniciar o ponto de acesso local.");
  }

  // A primeira busca sera executada logo no inicio.
  primeiraBuscaRoteadores = true;

  // Servidor web local
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatusLocal);
  server.on("/comando", HTTP_GET, handleComandoLocal);
  server.begin();

  // Estado inicial automatico
  comandoRemoto = 0;
  manutencaoLocal = false;

  atualizarEstadoVisual(true);

  // Tarefa independente para Wi-Fi e Firebase
  BaseType_t resultado = xTaskCreatePinnedToCore(
    tarefaRede,
    "TarefaRede",
    10000,
    NULL,
    1,
    NULL,
    0
  );

  if (resultado != pdPASS) {
    Serial.println("ERRO: nao foi possivel iniciar a tarefa de rede.");
  }

  Serial.println("Gym Pole Monitor iniciado.");
}

// =====================================================
// 23. LOOP PRINCIPAL
// =====================================================

void loop() {
  verificarBotao();
  processarEventoBotao();

  verificarSensor();

  server.handleClient();

  atualizarEstadoVisual();

  delay(1);
}
