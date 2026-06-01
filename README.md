# TCC

# Hardwares utilizados

- Esp32-C3 super mini
- Sensor DHT22

# Ferramentas utilizadas

- Arduino IDE

# Bibliotecas utilizadas

- DHT sensor library

# Conectando o Arduino IDE ao Esp32

- Adicionar a URL do ESP32 na IDE

    - Abra a Arduino IDE.
    - Vá no menu Arduino IDE > Ajustes....
    - No campo URLs Adicionais de Gerenciadores de Placas, cole o seguinte link:
        * `https://espressif.github.io/arduino-esp32/package_esp32_index.json`

- Instalar a placa no Gerenciador
    - Vá em Ferramentas > Placa > Gerenciador de Placas
    - Na barra de pesquisa, digite ESP32.
    - Encontre o pacote chamado esp32 (feito pela Espressif Systems) e clique em Instalar

- Selecionar a placa e a porta
    - Conecte o seu ESP32 ao computador usando um cabo USB que transmita dados, não apenas energia
    - Mantenha o botão boot pressionado, dê um clique no botão reset e solte o botão boot 
    - Vá em Ferramentas > Placa > ESP32
    - Selecione exatamente: "ESP32C3 Dev Module"
    - Selecione a porta correspondente (a nomeclatura muda de sistema para sistema, teste removendo e inserindo novamente o ESP32 e veja qual porta apareceu apenas com a placa conectada)
    - Vá em Ferramentas > USB CDC On Boot > Enabled

- Testando a placa com o blink
    - Cole o código abaixo na sua IDE:
    ```c++
    // O LED nativo da maioria das placas ESP32 está no pino 8
    #define LED_PIN 8

    void setup() {
        pinMode(LED_PIN, OUTPUT);
    }

    void loop() {
        digitalWrite(LED_PIN, HIGH); // Liga o LED
        delay(1000);                 // Aguarda 1 segundo
        digitalWrite(LED_PIN, LOW);  // Desliga o LED
        delay(1000);                 // Aguarda 1 segundo
    }
    ```
    - Clique no botão de seta para a direita (Carregar/Upload) no topo da IDE

- Instalando as libs
    - Vá em Ferramentas > Gerenciar Bibliotecas...
    - Busque por "DHT sensor library" (da Adafruit) e clique em instalar.
    - A IDE provavelmente vai pedir para instalar uma dependência chamada "Adafruit Unified Sensor". Clique em Instalar Todas (Install All).

- Testando o sensor
    - Cole o código abaixo na sua IDE:
    ```c++
    #include "DHT.h"

    #define DHTPIN 5

    #define DHTTYPE DHT22

    DHT dht(DHTPIN, DHTTYPE);

    unsigned long lastTime = 0;

    const unsigned long INTERVAL = 5000;

    void setup() {
        Serial.begin(115200);

        while (!Serial) {
            delay(10);
        }

        pinMode(DHTPIN, INPUT_PULLUP);
        dht.begin();
    }

    void loop() {
        unsigned long now = millis();

        if (now - lastTime >= INTERVAL) {
            lastTime = now; 

            float umidade = dht.readHumidity(true);
            float temperatura = dht.readTemperature(false, true);

            if (isnan(umidade) || isnan(temperatura)) {
            Serial.println(F("[Aviso] Falha de comunicação. Tentando novamente em breve..."));
            lastTime = now - (INTERVAL - 100);
            return;
            }

            Serial.print(F("Umidade: "));
            Serial.print(umidade, 1);
            Serial.print(F("%  |  Temperatura: "));
            Serial.print(temperatura, 1);
            Serial.println(F("°C"));

            Serial.flush(); 
        }
    }
    ```
    - Clique no botão de seta para a direita(Carregar/Upload) no topo da IDE