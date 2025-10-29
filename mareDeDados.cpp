#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Defina os pinos que você conectou
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

// Cria objeto do display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando display ST7789...");

  // Inicializa com resolução 240x240
  tft.init(240, 240);

  // Liga o backlight se precisar
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(20, 20);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.println("Ola ESP32!");
}

void loop() {
  // Animação simples
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 120);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(3);
  tft.println("Rodando...");
  delay(1000);

  tft.fillScreen(ST77XX_BLUE);
  delay(1000);
}