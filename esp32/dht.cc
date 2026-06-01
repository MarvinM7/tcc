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