#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <HardwareSerial.h>

// Configurações WiFi
const char* ssid = "SUA_REDE_WIFI";
const char* password = "SUA_SENHA_WIFI";

// Configurações do servidor/banco de dados
const char* serverURL = "http://seu-servidor.com/api/sensordata";
// Ou para localhost: "http://192.168.1.100:3000/api/data"

// Configuração DHT22
#define DHT_PIN 4
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// Configuração MP503 (Sensor de Qualidade do Ar)
#define MP503_PIN 34

// Comunicação serial com Arduino
HardwareSerial SerialArduino(2); // RX=16, TX=17

// Variáveis dos sensores
float temperature = 0;
float humidity = 0;
int co2 = 0;
int airQuality = 0;
unsigned long lastSend = 0;
const long sendInterval = 30000; // Enviar a cada 30 segundos

// Status do sistema
bool wifiConnected = false;
bool lastSendSuccess = true;

void setup() {
  Serial.begin(115200);
  SerialArduino.begin(9600, SERIAL_8N1, 16, 17);
  
  // Inicializar sensores
  dht.begin();
  pinMode(MP503_PIN, INPUT);
  
  // Conectar WiFi
  connectWiFi();
  
  Serial.println("Sistema de Sensores Iniciado");
  delay(2000);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Verificar conexão WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  
  // Processar dados do Arduino
  readSerialData();
  
  // Ler sensores locais a cada 5 segundos
  static unsigned long lastSensorRead = 0;
  if (currentMillis - lastSensorRead >= 5000) {
    lastSensorRead = currentMillis;
    readDHTSensor();
    readAirQualitySensor();
    displayLocalData();
  }
  
  // Enviar para banco de dados a cada 30 segundos
  if (currentMillis - lastSend >= sendInterval) {
    lastSend = currentMillis;
    
    if (sendToDatabase()) {
      Serial.println("✅ Dados enviados para o banco de dados");
      lastSendSuccess = true;
    } else {
      Serial.println("❌ Falha no envio para o banco de dados");
      lastSendSuccess = false;
    }
  }
  
  delay(100);
}

void connectWiFi() {
  Serial.println();
  Serial.print("Conectando à rede: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi conectado!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
  } else {
    Serial.println();
    Serial.println("❌ Falha na conexão WiFi!");
    wifiConnected = false;
  }
}

void readSerialData() {
  if (SerialArduino.available()) {
    String data = SerialArduino.readStringUntil('\n');
    data.trim();
    
    if (data.startsWith("CO2:")) {
      String co2Value = data.substring(4);
      if (co2Value != "ERROR") {
        co2 = co2Value.toInt();
        Serial.println("✅ CO2: " + String(co2) + " ppm");
      } else {
        Serial.println("❌ Erro na leitura do CO2");
        co2 = -1;
      }
    }
  }
}

void readDHTSensor() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Erro na leitura do DHT22!");
    temperature = -999;
    humidity = -999;
  }
}

void readAirQualitySensor() {
  int rawValue = analogRead(MP503_PIN);
  airQuality = map(rawValue, 0, 4095, 100, 0);
  airQuality = constrain(airQuality, 0, 100);
}

bool sendToDatabase() {
  if (!wifiConnected) {
    Serial.println("WiFi não conectado");
    return false;
  }
  
  HTTPClient http;
  
  // Configurar cabeçalhos HTTP
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP32-Sensor-Station");
  
  // Criar JSON para envio
  String jsonData = createJSON();
  Serial.println("Enviando dados: " + jsonData);
  
  // Fazer requisição POST
  int httpResponseCode = http.POST(jsonData);
  
  bool success = false;
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Código HTTP: " + String(httpResponseCode));
    Serial.println("Resposta: " + response);
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      success = true;
    }
  } else {
    Serial.println("Erro na requisição: " + String(httpResponseCode));
    success = false;
  }
  
  http.end();
  return success;
}

String createJSON() {
  StaticJsonDocument<512> doc;
  
  // Dados básicos
  doc["device_id"] = "ESP32_Sensor_01";
  doc["timestamp"] = getTimestamp();
  doc["location"] = "Sala_01";
  
  // Dados dos sensores
  JsonObject sensors = doc.createNestedObject("sensors");
  
  // DHT22
  JsonObject dht22 = sensors.createNestedObject("dht22");
  dht22["temperature"] = temperature;
  dht22["humidity"] = humidity;
  dht22["status"] = (temperature != -999) ? "ok" : "error";
  
  // MH-Z14A (CO2)
  JsonObject co2_sensor = sensors.createNestedObject("mhz14a");
  co2_sensor["co2_ppm"] = co2;
  co2_sensor["status"] = (co2 > 0) ? "ok" : "error";
  
  // MP503 (Qualidade do Ar)
  JsonObject air_quality = sensors.createNestedObject("mp503");
  air_quality["quality_percent"] = airQuality;
  air_quality["level"] = getAirQualityLevel(airQuality);
  air_quality["status"] = "ok";
  
  // Status do sistema
  doc["wifi_strength"] = WiFi.RSSI();
  doc["system_status"] = "operational";
  
  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

String getTimestamp() {
  // Simulação de timestamp - em produção usar NTP
  unsigned long now = millis();
  unsigned long hours = (now / 3600000) % 24;
  unsigned long minutes = (now / 60000) % 60;
  unsigned long seconds = (now / 1000) % 60;
  
  String timestamp = "2024-" + String((now / 86400000) % 30 + 1) + 
                    "-" + String((now / 2592000000) % 12 + 1) + 
                    " " + String(hours) + ":" + 
                    String(minutes) + ":" + String(seconds);
  return timestamp;
}

String getAirQualityLevel(int quality) {
  if (quality >= 80) return "excellent";
  else if (quality >= 60) return "good";
  else if (quality >= 40) return "moderate";
  else if (quality >= 20) return "poor";
  else return "very_poor";
}

void displayLocalData() {
  Serial.println("=== DADOS LOCAIS ===");
  Serial.println("Temperatura: " + String(temperature) + "°C");
  Serial.println("Umidade: " + String(humidity) + "%");
  Serial.println("CO2: " + String(co2) + " ppm");
  Serial.println("Qualidade Ar: " + String(airQuality) + "%");
  Serial.println("WiFi: " + String(WiFi.RSSI()) + " dBm");
  Serial.println("====================");
}