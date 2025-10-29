
#include <HardwareSerial.h>

HardwareSerial SerialMHZ(2);  // Usando UART2 do ESP32

// Pinos para UART2 (pode variar dependendo do modelo do ESP32)
#define RX_PIN 16
#define TX_PIN 17

// Comandos para o sensor
byte cmd_read_co2[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte response[9];

void setup() {
  Serial.begin(115200);
  SerialMHZ.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  
  Serial.println("Iniciando sensor MH-Z14A...");
  delay(3000);  // Aguarda inicialização do sensor
}

void loop() {
  int co2_ppm = readCO2();
  
  if (co2_ppm > 0) {
    Serial.print("Concentração de CO2: ");
    Serial.print(co2_ppm);
    Serial.println(" ppm");
  } else {
    Serial.println("Erro na leitura do sensor");
  }
  
  delay(5000);  // Leitura a cada 5 segundos
}

int readCO2() {
  // Envia comando de leitura
  SerialMHZ.write(cmd_read_co2, 9);
  SerialMHZ.flush();
  
  // Aguarda resposta
  delay(100);
  
  // Lê resposta
  if (SerialMHZ.available() >= 9) {
    SerialMHZ.readBytes(response, 9);
    
    // Verifica checksum
    if (verifyChecksum(response)) {
      int high = response[2];
      int low = response[3];
      return high * 256 + low;
    }
  }
  return -1;
}

bool verifyChecksum(byte packet[]) {
  byte checksum = 0;
  for (int i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xFF - checksum;
  checksum += 1;
  
  return checksum == packet[8];
}