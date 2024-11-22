#include <WiFi.h>
#include <PubSubClient.h>


// Configurações - variáveis editáveis
const char* default_SSID = "Wokwi-GUEST"; // Nome da rede Wi-Fi
const char* default_PASSWORD = ""; // Senha da rede Wi-Fi
const char* default_BROKER_MQTT = ""; // IP do Broker MQTT
const int default_BROKER_PORT = 1883; // Porta do Broker MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp001/cmd"; // Tópico MQTT de escuta
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp001/attrs"; // Tópico para envio de estado
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp001/attrs/l"; // Tópico para luminosidade
const char* default_TOPICO_PUBLISH_3 = "/TEF/lamp001/attrs/f"; // Tópico para fluxo
// const char* default_TOPICO_PUBLISH_4 = "/TEF/potato009/attrs/"; // Tópico para 
const char* default_ID_MQTT = "fiware_001"; // ID do cliente MQTT
const int default_D4 = 2; // Pino do LED onboard

WiFiClient espClient; // Cliente Wi-Fi
PubSubClient MQTT(espClient); // Cliente MQTT
char EstadoSaida = '0'; // Estado da saída (LED)

//flux
#define FS300A_PULSE     508         // PULSE / LITRO
#define FS300A_FLOW_RATE 60          // LITRO / MINUTO
const float factor = 60.0F / 508.0F;  // FS300A_FLOW_RATE / FS300A_PULSE

#define interruptPin 5

volatile uint16_t pulse;  // Variável que será incrementada na interrupção
uint16_t count;           // Variável para armazenar o valor atual de pulse

float frequency;          // Frequência calculada a partir de count
float flowRate;           // Taxa de fluxo calculada a partir da frequência

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;  // Mutex para garantir acesso seguro a pulse

void IRAM_ATTR FlowInterrupt() {
  portENTER_CRITICAL_ISR(&mux);  // Entra em uma seção crítica de interrupção
  pulse++;  // Incrementa a variável pulse de maneira segura
  portEXIT_CRITICAL_ISR(&mux);   // Sai da seção crítica de interrupção
}
//

void initSerial() {
    Serial.begin(115200); // Inicializa a comunicação serial a 115200 bps
}

void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(default_SSID);
    Serial.println("Aguarde");
    reconectWiFi(); // Chama a função para conectar ao Wi-Fi
}

void initMQTT() {
    MQTT.setServer(default_BROKER_MQTT, default_BROKER_PORT); // Configura o broker MQTT
    MQTT.setCallback(mqtt_callback); // Define a função de callback para mensagens recebidas
}

void setup() {
    InitOutput(); // Inicializa a saída (LED)
    initSerial(); // Inicializa a comunicação serial
    initWiFi(); // Conecta ao Wi-Fi
    initMQTT(); // Inicializa a conexão MQTT
    pinMode(interruptPin, INPUT_PULLUP); // Configura o pino do sensor de fluxo
    attachInterrupt(digitalPinToInterrupt(interruptPin), FlowInterrupt, CHANGE); // Configura a interrupção
    delay(5000);
    MQTT.publish(default_TOPICO_PUBLISH_1, "s|on"); // Publica que o sistema está ligado
}

void loop() {
    VerificaConexoesWiFIEMQTT(); // Verifica as conexões Wi-Fi e MQTT
    EnviaEstadoOutputMQTT(); // Envia o estado da saída via MQTT
    handleLuminosity(); // Lida com a leitura da luminosidade
    handleFlow(); // Lida com o cálculo de fluxo de água
    MQTT.loop(); // Mantém a conexão MQTT ativa
}

void handleFlow() {
    static unsigned long startTime;
    if (micros() - startTime < 1000000UL) return; // Intervalo de 1 segundo
    startTime = micros();

    portENTER_CRITICAL(&mux); // Entra em uma seção crítica
    count = pulse; // Salva o valor atual de pulse e zera pulse
    pulse = 0;
    portEXIT_CRITICAL(&mux); // Sai da seção crítica

    frequency = count / 2.0f; // Calcula a frequência
    flowRate = frequency * factor; // Calcula a taxa de fluxo
    String mensagem = String(flowRate, 3); // Formata o valor com 3 casas decimais
    Serial.print("Fluxo de água: ");
    Serial.println(mensagem); // Exibe o valor no Serial Monitor
    MQTT.publish(default_TOPICO_PUBLISH_3, mensagem.c_str()); // Publica o valor do fluxo
}

void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return; // Se já estiver conectado, não faz nada
    WiFi.begin(default_SSID, default_PASSWORD); // Tenta conectar ao Wi-Fi
    while (WiFi.status() != WL_CONNECTED) { // Aguarda conexão
        delay(100);
        Serial.print("."); // Exibe ponto enquanto aguarda
    }
    Serial.println();
    Serial.println("Conectado com sucesso na rede ");
    Serial.print(default_SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP()); // Exibe o IP obtido
    digitalWrite(default_D4, LOW); // Garante que o LED inicie desligado
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg; // String para armazenar a mensagem recebida
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i]; // Converte o payload para char
        msg += c; // Adiciona o char à string da mensagem
    }
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg); // Exibe a mensagem recebida

    // Controla o LED com base na mensagem recebida
    if (msg.equals(String("EnvM001@on|"))) {
        digitalWrite(default_D4, HIGH); // Liga o LED
        EstadoSaida = '1'; // Atualiza o estado
    } else if (msg.equals(String("EnvM001@off|"))) {
        digitalWrite(default_D4, LOW); // Desliga o LED
        EstadoSaida = '0'; // Atualiza o estado
    }
}

void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT(); // Reconecta se não estiver conectado ao MQTT
    reconectWiFi(); // Verifica a conexão Wi-Fi
}

void EnviaEstadoOutputMQTT() {
    if (EstadoSaida == '1') {
        MQTT.publish(default_TOPICO_PUBLISH_1, "s|on"); // Publica que o LED está ligado
        Serial.println("- Led Ligado");
    } else {
        MQTT.publish(default_TOPICO_PUBLISH_1, "s|off"); // Publica que o LED está desligado
        Serial.println("- Led Desligado");
    }
    Serial.println("- Estado do LED onboard enviado ao broker!"); // Confirma o envio do estado
    delay(1000); // Espera 1 segundo antes de repetir
}

void InitOutput() {
    pinMode(default_D4, OUTPUT); // Configura o pino do LED como saída
    digitalWrite(default_D4, HIGH); // Inicia com o LED desligado
}

void reconnectMQTT() {
    while (!MQTT.connected()) { // Tenta reconectar enquanto não estiver conectado
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(default_BROKER_MQTT);
        if (MQTT.connect(default_ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!"); // Confirma a conexão
            MQTT.subscribe(default_TOPICO_SUBSCRIBE); // Inscreve-se no tópico de comando
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000); // Espera 2 segundos antes da próxima tentativa
        }
    }
}

void handleLuminosity() {
    const int potPin = 34; // Pino do sensor de luminosidade
    int sensorValue = analogRead(potPin); // Lê o valor do sensor
    int luminosity = map(sensorValue, 0, 4095, 0, 100); // Mapeia o valor para 0-100
    String mensagem = String(luminosity); // Converte o valor para string
    Serial.print("Valor da luminosidade: ");
    Serial.println(mensagem.c_str()); // Exibe o valor da luminosidade
    MQTT.publish(default_TOPICO_PUBLISH_2, mensagem.c_str()); // Publica o valor da luminosidade
}
