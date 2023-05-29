#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "oledDisplay.h"

#define SCREEN_WIDTH 128  // OLED display width,  in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

OledDisplay::OledDisplay() {
  _oled = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
}

OledDisplay::~OledDisplay() {
  delete _oled;
}

void OledDisplay::begin() {
  if (!_oled->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;
  }

  delay(2000);         // wait for initializing
  _oled->clearDisplay(); // clear display

  _oled->setTextSize(1);          // text size
  _oled->setTextColor(WHITE);     // text color
  _oled->setCursor(0, 10);
}

void OledDisplay::showTemperatureAndHumidity(ThermometerData data) {
  _oled->setCursor(0, 1);
  _oled->print(data.temperature);
  _oled->print((char)223);
  _oled->print("C ");
  _oled->print(data.humidity);
  _oled->print("%");
  _oled->print("     ");
}

void OledDisplay::dispalyTimerStatus(bool isRelayOn, int delaySeconds, int elapsedSeconds) {
  _oled->setCursor(0, 0);
  if (!isRelayOn) {
    _oled->print("Ready(");
    _oled->print(delaySeconds);
    _oled->print(")");
  } else {
    _oled->print("Running(");
    _oled->print(delaySeconds - elapsedSeconds);
    _oled->print(")");
  }
  _oled->print("                ");
}