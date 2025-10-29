#include "DHT.h"

#define DHTPIN 13    // pino digital onde o DHT22 está ligado
#define DHTTYPE DHT22   // tipo do sensor (DHT22)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando leitura do DHT22...");
  dht.begin();
}

void loop() {
  // Espera um pouco entre as leituras
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Verifica se houve falha na leitura
  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao ler do sensor DHT22!");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print(" %  |  Temperatura: ");
  Serial.print(t);
  Serial.println(" °C");
}

