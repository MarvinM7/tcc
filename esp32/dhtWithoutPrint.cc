#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// --- Configurações do Wi-Fi ---
const char* ssid = "";
const char* password = "";

// --- Configurações do Node-RED / Mosquitto ---
const char* nr_server = "";
const int nr_port = 1883;
const char* nr_topic = "";

// --- Configurações do ThingsBoard ---
const char* tb_server = "";
const int tb_port = 1884;
const char* tb_token = "";
const char* tb_topic = "v1/devices/me/telemetry";

// --- Configurações do Sensor DHT22 ---
#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --- Instâncias de rede ---
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
unsigned long delayInterval = 5000;

void setup_wifi() { 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int loading = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  delay(1000);
}

// Função dedicada para enviar ao Mosquitto (Node-RED)
void sendToNodeRed(char* jsonBuffer) {
  client.setServer(nr_server, nr_port);
  
  String clientId = "ESP32-NodeRed-" + String(random(0, 1000));
  
  if (client.connect(clientId.c_str())) {
    client.publish(nr_topic, jsonBuffer);
    client.disconnect();
  } else {
    delay(1000);
  }
}

// Função dedicada para enviar ao ThingsBoard
void sendToThingsBoard(char* jsonBuffer) {
  client.setServer(tb_server, tb_port);
  
  String clientId = "ESP32-Quarto-ThingsBoard";
  
  if (client.connect(clientId.c_str(), tb_token, NULL)) {
    client.publish(tb_topic, jsonBuffer);
    client.disconnect();
  } else {
    delay(1000);
  }
}

void setup() {
  setup_wifi();
  
  dht.begin();
}

void loop() {
  // Mantém o Wi-Fi vivo se cair
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  unsigned long now = millis();
  if (now - lastMsg > delayInterval) {
    lastMsg = now;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      return;
    }

    // Montagem do JSON
    StaticJsonDocument<256> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["heap_livre"] = ESP.getFreeHeap();
    char buffer[256];
    serializeJson(doc, buffer);

    // Envia sequencialmente para os dois barramentos isolados
    sendToNodeRed(buffer);
    sendToThingsBoard(buffer);
  }
}