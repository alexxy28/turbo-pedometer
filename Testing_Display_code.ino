#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>

// GC9A01 wiring
#define TFT_BLK  40
#define TFT_CS   38
#define TFT_SCL  36
#define TFT_SDA  34
#define TFT_DC   21
#define TFT_RES  17

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RES);

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("GC9A01 ESP32 SPI LCD Test");

  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, HIGH);   // backlight ON

  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS);
  tft.begin();

  tft.setRotation(0);
  tft.fillScreen(GC9A01A_BLACK);

  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(3);
  tft.setCursor(45, 80);
  tft.println("ESP32");

  tft.setTextSize(2);
  tft.setCursor(45, 125);
  tft.println("GC9A01 OK");

  delay(1500);
}

void loop() {
  tft.fillScreen(GC9A01A_RED);
  delay(500);

  tft.fillScreen(GC9A01A_GREEN);
  delay(500);

  tft.fillScreen(GC9A01A_BLUE);
  delay(500);

  tft.fillScreen(GC9A01A_BLACK);
  tft.drawCircle(120, 120, 100, GC9A01A_WHITE);
  tft.fillCircle(120, 120, 60, GC9A01A_YELLOW);

  tft.setTextColor(GC9A01A_BLACK);
  tft.setTextSize(2);
  tft.setCursor(75, 112);
  tft.println("WORKS");

  delay(1500);
}