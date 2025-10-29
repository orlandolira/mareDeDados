#include <SoftwareSerial.h>

// ==============================================
// CONFIGURAÇÕES HARDWARE - DEFINA OS PINOS AQUI!
// ==============================================
// RX do Arduino (pino 10) <-- TX do Sensor (Azul)
// TX do Arduino (pino 11) --> RX do Sensor (Amarelo)
SoftwareSerial SerialMHZ(10, 11); // (RX, TX)

// ==============================================
// VARIÁVEIS GLOBAIS
// ==============================================
// Comando para solicitar leitura de CO2 (0x86)
byte comandoLeitura[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
// Array para armazenar a resposta do sensor (9 bytes)
byte resposta[9];

// ==============================================
// FUNÇÃO SETUP (EXECUTADA UMA VEZ AO LIGAR)
// ==============================================
void setup() {
  // Inicia a comunicação serial com o PC (Monitor Serial)
  Serial.begin(9600);
  // Inicia a comunicação com o sensor MH-Z14A
  SerialMHZ.begin(9600);

  Serial.println("==================================");
  Serial.println("   SENSOR DE CO2 MH-Z14A");
  Serial.println("==================================");
  Serial.println("Iniciando...");
  
  // Aguarda o período de aquecimento inicial de 3 minutos
  Serial.println("Aguarde 3 minutos para aquecimento do sensor.");
  for (int i = 3; i > 0; i--) {
    Serial.print("Tempo restante: ");
    Serial.print(i);
    Serial.println(" minuto(s).");
    delay(60000); // Espera 1 minuto (60000 ms)
  }
  Serial.println("Aquecimento concluído. Iniciando leituras.");
  Serial.println("----------------------------------");
}

// ==============================================
// FUNÇÃO LOOP (EXECUTADA REPETIDAMENTE)
// ==============================================
void loop() {
  unsigned int ppm = lerSensorCO2();
  
  // Exibe o valor no Monitor Serial
  Serial.print("Concentração de CO₂: ");
  Serial.print(ppm);
  Serial.println(" ppm");

  // Classifica a qualidade do ar baseado no CO2
  classificarQualidadeAr(ppm);
  
  Serial.println("----------------------------------");
  
  // Espera 5 segundos entre as leituras
  delay(5000);
}

// ==============================================
// FUNÇÃO PARA LER O SENSOR
// ==============================================
unsigned int lerSensorCO2() {
  // 1. Limpa o buffer de entrada ANTES de enviar o comando
  while (SerialMHZ.available() > 0) {
    SerialMHZ.read();
  }

  // 2. Envia o comando de leitura para o sensor
  SerialMHZ.write(comandoLeitura, 9);
  
  // 3. Espera o sensor responder
  delay(200); // tempo de resposta seguro

  // 4. Verifica se há dados disponíveis
  if (SerialMHZ.available() >= 9) {
    SerialMHZ.readBytes(resposta, 9);

    // 5. Verifica se o cabeçalho é válido
    if (resposta[0] == 0xFF && resposta[1] == 0x86) {
      // 6. Calcula e valida o checksum
      byte checksumCalculado = calcularChecksum(resposta);
      if (checksumCalculado == resposta[8]) {
        // 7. Retorna o valor de CO2
        return (resposta[2] * 256) + resposta[3];
      } else {
        Serial.println("Erro: Checksum inválido. Dados corrompidos.");
      }
    } else {
      Serial.println("Erro: Resposta inválida do sensor.");
    }
  } else {
    Serial.println("Erro: Nenhuma resposta do sensor. Verifique as conexões.");
  }
  
  return 0; // Em caso de erro
}

// ==============================================
// FUNÇÃO PARA CALCULAR CHECKSUM
// ==============================================
byte calcularChecksum(byte pacote[9]) {
  byte soma = 0;
  // Soma do byte[1] ao byte[7] (índices 1 a 7 do array)
  for (int i = 1; i < 8; i++) {
    soma += pacote[i];
  }
  // Fórmula padrão do checksum do MH-Z14A
  return (0xFF - soma) + 0x01;
}

// ==============================================
// FUNÇÃO PARA CLASSIFICAR A QUALIDADE DO AR
// ==============================================
void classificarQualidadeAr(unsigned int ppm) {
  Serial.print("Qualidade do Ar: ");
  
  if (ppm <= 400) {
    Serial.println("Ar de Referência (Exterior)");
  } else if (ppm <= 600) {
    Serial.println("Excelente");
  } else if (ppm <= 800) {
    Serial.println("Boa");
  } else if (ppm <= 1000) {
    Serial.println("Moderada");
  } else if (ppm <= 1500) {
    Serial.println("Ruim - Ventile o Ambiente!");
  } else if (ppm <= 2000) {
    Serial.println("Muito Ruim - Ventile IMEDIATAMENTE!");
  } else {
    Serial.println("PERIGOSA - Evite respirar neste ambiente!");
  }
}
