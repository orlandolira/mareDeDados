#include <DHT.h>
#include <HardwareSerial.h>

// Configuração DHT22
#define DHT_PIN 4
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// Configuração MP503 (Sensor de Qualidade do Ar)
#define MP503_PIN A0

// Comunicação serial com Arduino
HardwareSerial SerialArduino(2); // UART2

// Variáveis para armazenar dados
float temperature = 0;
float humidity = 0;
int co2 = 0;
int airQuality = 0;

unsigned long previousMillis = 0;
const long interval = 5000; // Intervalo de 5 segundos

void setup() {
  // Inicializar comunicação serial
  Serial.begin(115200); // Para monitor serial
  SerialArduino.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  
  // Inicializar sensores
  dht.begin();
  pinMode(MP503_PIN, INPUT);
  
  Serial.println("ESP32_SENSORS_READY");
  
  // Aguardar Arduino estar pronto
  delay(2000);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Processar dados recebidos do Arduino
  readSerialData();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Ler sensores locais
    readDHTSensor();
    readAirQualitySensor();
    
    // Exibir todos os dados
    displayAllData();
  }
}

void readSerialData() {
  if (SerialArduino.available()) {
    String data = SerialArduino.readStringUntil('\n');
    data.trim();
    
    if (data.startsWith("CO2:")) {
      co2 = data.substring(4).toInt();
      Serial.println("Dados CO2 recebidos: " + String(co2) + " ppm");
    }
  }
}

void readDHTSensor() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Erro na leitura do DHT22!");
    temperature = 0;
    humidity = 0;
  }
}

void readAirQualitySensor() {
  int rawValue = analogRead(MP503_PIN);
  // Converter para valor de qualidade do ar (0-100%)
  // Valores mais baixos = melhor qualidade
  airQuality = map(rawValue, 0, 4095, 100, 0);
}

void displayAllData() {
  Serial.println("=== DADOS DOS SENSORES ===");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  Serial.print("CO2: ");
  Serial.print(co2);
  Serial.println(" ppm");
  
  Serial.print("Qualidade do Ar: ");
  Serial.print(airQuality);
  Serial.println(" %");
  
  // Classificar qualidade do ar
  Serial.print("Status: ");
  if (airQuality >= 80) Serial.println("EXCELENTE");
  else if (airQuality >= 60) Serial.println("BOA");
  else if (airQuality >= 40) Serial.println("MODERADA");
  else if (airQuality >= 20) Serial.println("RUIM");
  else Serial.println("PÉSSIMA");
  
  Serial.println("===========================");
}