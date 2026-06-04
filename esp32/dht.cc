#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "MVT_RENATA_2.4";
const char* password = "68764495";

const char* mqtt_server = "192.168.1.135";
const int mqtt_port = 1883;
const char* mqtt_topic = "quarto-dht22";
//const char* mqtt_topic = "escritorio-dht22";

#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
unsigned long delayInterval = 5000; 

void setup_wifi() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("\n--- Iniciando Diagnóstico de Wi-Fi ---");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    tentativas++;

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWi-Fi Conectado com sucesso!");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
    } else if (tentativas > 30) {
      tentativas = 0;
      Serial.println("\n❌ Falha na conexão.");
      Serial.print("Código de Status do Wi-Fi: ");
      Serial.println(WiFi.status());
      
      // 1 = WL_NO_SSID_AVAIL (Não encontrou a rede/rede 5G)
      // 4 = WL_CONNECT_FAILED (Falhou ao conectar)
      // 5 = WL_CONNECTION_LOST (Conexão perdida)
      // 6 = WL_DISCONNECTED (Senha errada ou rejeitado pelo roteador)
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32-MarvinTCC-" + String(random(0, 1000));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado ao Mosquitto!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
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
      Serial.println("Falha ao ler o sensor DHT!");
      return;
    }

    StaticJsonDocument<200> doc;
    doc["temperatura"] = temperatura;
    doc["umidade"] = umidade;
    doc["heap_livre"] = ESP.getFreeHeap();

    char buffer[256];
    serializeJson(doc, buffer);

    Serial.print("Publicando payload: ");
    Serial.println(buffer);
    if(client.publish(mqtt_topic, buffer)) {
      Serial.println("Envio realizado com sucesso!");
    } else {
      Serial.println("Erro ao publicar no Mosquitto.");
    }
  }
}