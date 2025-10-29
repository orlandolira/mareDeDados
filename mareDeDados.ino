#include <DHT.h>

#define DHTPIN 13      // Pino do sensor DHT22 (D26)
#define DHTTYPE DHT22  // Tipo do sensor DHT
#define ZP07_PIN 12   // Pino analógico para o sensor ZP07-4 (D25)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);  // Inicializa o monitor serial
  Serial.println("Iniciando o ESP32...");

  dht.begin();           // Inicializa o sensor DHT22
  Serial.println("Sensor DHT22 iniciado.");

  pinMode(ZP07_PIN, INPUT); // Configura o pino do sensor ZP07-4 como entrada
  Serial.println("Sensor ZP07-4 configurado.");
}

void loop() {
  Serial.println("Lendo sensores...");

  // Leitura do sensor DHT22
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Verifica se houve falha na leitura do DHT22
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha na leitura do sensor DHT22!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C | Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");
  }

  // Leitura do sensor ZP07-4 (gás)
  int gasValue = analogRead(ZP07_PIN);  // Leitura analógica do sensor de gás
  float voltagemGas = gasValue * (3.3 / 4095.0); // Converte para voltagem (valor de 12 bits no ESP32)

  // Imprime os dados do sensor ZP07-4
  Serial.print("Gás - Valor bruto: ");
  Serial.print(gasValue);
  Serial.print(" | Tensão: ");
  Serial.print(voltagemGas);
  Serial.println(" V");

  delay(2000);  // Atraso de 2 segundos entre as leituras
}

