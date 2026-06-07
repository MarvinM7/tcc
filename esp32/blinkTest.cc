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