#include <SoftwareSerial.h>

// Configuração do sensor MH-Z14A
// RX do Arduino conectado no T (TX) do sensor
// TX do Arduino conectado no R (RX) do sensor
SoftwareSerial co2Serial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600); // Comunicação com ESP32
  co2Serial.begin(9600); // Sensor MH-Z14A
  
  // Aguardar sensor inicializar
  delay(3000);
  
  // Inicializar sensor CO2
  initMHZ14A();
  
  Serial.println("ARDUINO_CO2_READY");
}

void initMHZ14A() {
  // Comando para iniciar leituras automáticas
  byte cmd[9] = {0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6};
  co2Serial.write(cmd, 9);
  delay(100);
}

int readCO2() {
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  byte response[9];
  int ppm = -1;
  
  // Enviar comando de leitura
  co2Serial.write(cmd, 9);
  delay(100);
  
  // Ler resposta
  if (co2Serial.available() >= 9) {
    co2Serial.readBytes(response, 9);
    
    // Verificar se é uma resposta válida
    if (response[0] == 0xFF && response[1] == 0x86) {
      ppm = (response[2] << 8) | response[3];
    }
  }
  
  return ppm;
}

void loop() {
  static unsigned long previousMillis = 0;
  const long interval = 5000;
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Ler sensor CO2
    int co2 = readCO2();
    
    if (co2 > 0) {
      // Enviar dados para ESP32
      Serial.print("CO2:");
      Serial.println(co2);
    } else {
      Serial.println("CO2:ERROR");
    }
  }
}