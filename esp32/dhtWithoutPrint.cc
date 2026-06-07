#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "";
const char* password = "";

const char* mqtt_server = "";
const int mqtt_port = 1883;
const char* mqtt_topic = "";

#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
unsigned long delayInterval = 5000; 

void setup_wifi() { 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32-MarvinTCC-" + String(random(0, 1000));
    
    if (!client.connect(clientId.c_str())) {
      delay(5000);
    }
  }
}

void setup() {
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > delayInterval) {
    lastMsg = now;

    float umidade = dht.readHumidity();
    float temperatura = dht.readTemperature();

    if (isnan(umidade) || isnan(temperatura)) {
      return;
    }

    StaticJsonDocument<200> doc;
    doc["temperatura"] = temperatura;
    doc["umidade"] = umidade;
    doc["heap_livre"] = ESP.getFreeHeap();

    char buffer[256];
    serializeJson(doc, buffer);

    client.publish(mqtt_topic, buffer);
  }
}