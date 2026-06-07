#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configurações do Wi-Fi ---
const char* ssid = "";
const char* password = "";

// --- Configurações do MQTT ---
const char* mqtt_server = "";
const int mqtt_port = 1883;
const char* mqtt_topic = "";

// --- Configuração da posição inicial do display ---
const int offsetY = 0;
const int offsetX = 28;

const int breakOneLineSize1 = 11;
const int breakTwoLinesSize1 = breakOneLineSize1 * 2;
const int breakOneLineSize2 = 17;


// --- Configurações do Sensor DHT22 ---
#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// --- Configurações do display OLED 0.42 ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 5
#define OLED_SCL 6
#define OLED_RESET -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Instâncias de rede ---
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
unsigned long delayInterval = 5000; 

void showOnDisplay(const String& linha1, const String& linha2, const String& linha3) {
  display.clearDisplay();
  display.setTextSize(1);
  
  display.setCursor(offsetX, offsetY);
  display.println(linha1);
  display.setCursor(offsetX, offsetY + breakOneLineSize1);
  display.println(linha2);
  display.setCursor(offsetX, offsetY + breakTwoLinesSize1);
  display.println(linha3);
  display.display();
}

void setup_wifi() { 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int loading = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    loading++;
    String dots = "";
    for (int i = 0; i < (loading % 4); i++) {
      dots += ".";
    }
    
    showOnDisplay("Wi-Fi", "Conectando", dots);
  }
  
  showOnDisplay("Wi-Fi", "Conectado!", "");
  delay(1000);
}

void reconnect() {
  while (!client.connected()) {
    showOnDisplay("MQTT", "Conectando...", "");
    String clientId = "ESP32-MarvinTCC-" + String(random(0, 1000));
    
    if (client.connect(clientId.c_str())) {
      showOnDisplay("MQTT", "Conectado!", "");
      delay(500);
    } else {
      showOnDisplay("MQTT", "Erro", "rc=" + String(client.state()));
      delay(5000);
    }
  }
}

void setup() {
  // Inicializa o I2C nos pinos corretos
  Wire.begin(OLED_SDA, OLED_SCL);

  // Inicializa a tela em modo de compatibilidade básica
  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    
    // Reconfigurar a resolução física para 72x40
    display.ssd1306_command(SSD1306_SETMULTIPLEX);
    display.ssd1306_command(0x27); // Altera o multiplex para 40 linhas (0x27)
    
    display.ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    display.ssd1306_command(0x00); // Zera o offset vertical
    
    display.ssd1306_command(SSD1306_SETSTARTLINE | 0x00); // Linha inicial 0
    
    display.ssd1306_command(SSD1306_COMSCANDEC); // Direção de escaneamento dos pinos comuns
    
    display.ssd1306_command(SSD1306_SETCOMPINS);
    display.ssd1306_command(0x12); // Configuração sequencial dos pinos COM
    // ------------------------------------------------------

    display.clearDisplay(); // Limpa display
    display.setTextSize(1); // Tamanho da fonte             
    display.setTextColor(SSD1306_WHITE);
    display.display();
  }

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

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      showOnDisplay("Sensor", "Erro", "DHT22");
      return;
    }

    display.clearDisplay(); // Limpa display
    display.setTextSize(2); // Tamanho da fonte
    
    display.setCursor(offsetX, offsetY);
    display.printf("T:%.1fC\n", temperature);
    display.setCursor(offsetX, offsetY + 17);
    display.printf("U:%.1f%%\n", humidity);
    display.display();

    // Envio do JSON via MQTT
    StaticJsonDocument<200> doc;
    doc["temperatura"] = temperature;
    doc["umidade"] = humidity;
    doc["heap_livre"] = ESP.getFreeHeap();

    char buffer[256];
    serializeJson(doc, buffer);

    client.publish(mqtt_topic, buffer);
  }
}