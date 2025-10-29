#include <HardwareSerial.h>

HardwareSerial MHZSerial(2); // UART2

void setup() {
  Serial.begin(115200); // Serial Monitor
  MHZSerial.begin(9600, SERIAL_8N1, 21, 22); // RX=21, TX=22

  Serial.println("Sensor MH-Z14A aquecendo... aguarde 3 minutos");
  delay(180000); // tempo de aquecimento
  Serial.println("Leitura iniciada!");
}

void loop() {
  byte cmd[9] = {0xFF,0x01,0x86,0,0,0,0,0,0x79};
  byte resp[9];

  MHZSerial.write(cmd, 9);
  delay(10);

  int i = 0;
  while(MHZSerial.available() && i<9){
    resp[i] = MHZSerial.read();
    i++;
  }

  if(i==9 && resp[0]==0xFF && resp[1]==0x86){
    int co2 = (resp[2]<<8) | resp[3];
    Serial.print("CO2: ");
    Serial.println(co2);
  } else {
    Serial.println("Nenhum dado válido...");
  }

  delay(2000);
}

