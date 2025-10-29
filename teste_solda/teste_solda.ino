// LED piscando com ESP32
#define LED_BUILTIN 2  // GPIO2 - LED interno na maioria das placas ESP32

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("ESP32 - LED Piscando Iniciado!");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // Liga o LED
  Serial.println("LED LIGADO");
  delay(1000);                       // Espera 1 segundo
  
  digitalWrite(LED_BUILTIN, LOW);    // Desliga o LED
  Serial.println("LED DESLIGADO");
  delay(1000);                       // Espera 1 segundo
}