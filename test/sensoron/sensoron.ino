#include <DHT.h>

#define DHTPIN 13 // Pino do sensor DHT22
#define DHTTYPE DHT22  // Tipo do sensor DHT
#define ZP07_PIN 12   // Pino analógico para o sensor ZP07-4 (pino válido para o ESP32)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);  // Inicializa o monitor serial
  dht.begin();           // Inicializa o sensor DHT22

  pinMode(ZP07_PIN, INPUT); // Configura o pino do sensor ZP07-4 como entrada
}

void loop() {
  // Leitura do sensor DHT22
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Leitura do sensor ZP07-4 (gás)
  int gasValue = analogRead(ZP07_PIN);  // Leitura analógica do sensor de gás
  float voltagemGas = gasValue * (3.3 / 4095.0); // Converte para voltagem (valor de 12 bits no ESP32)

  // Verificação de falhas de leitura
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha na leitura do sensor DHT22!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C | Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");
  }

  // Imprime os dados do sensor ZP07-4
  Serial.print("Gás - Valor bruto: ");
  Serial.print(gasValue);
  Serial.print(" | Tensão: ");
  Serial.print(voltagemGas);
  Serial.println(" V");

  delay(2000);  // Atraso de 2 segundos entre as leituras
}