#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "oledDisplay.h"
#include "utils.h";

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

  delay(2000);            // wait for initializing
  _oled->clearDisplay();  // clear display

  _oled->setTextSize(2);       // text size
  _oled->setTextColor(WHITE);  // text color
  _oled->setCursor(0, 10);
}

void OledDisplay::showTemperatureAndHumidity(ThermometerData data) {
  _oled->setCursor(0, 18);
  _oled->print((int)data.temperature);
  _oled->print((char)247);
  _oled->print("C ");
  _oled->print((int)data.humidity);
  _oled->print("%");
  _oled->print("     ");
  _oled->display();

  Serial.print(data.temperature);
  Serial.print((char)223);
  Serial.print("C ");
  Serial.print(data.humidity);
  Serial.println("%");
}

void OledDisplay::dispalyTimerStatus(bool isRelayOn, int delaySeconds, int elapsedSeconds) {
  _oled->dim(true);
  _oled->clearDisplay();
  _oled->setCursor(0, 0);
  if (!isRelayOn) {
    _oled->print("  ");
    _oled->print(secondsToTimeString(delaySeconds));

    Serial.print("Ready(");
    Serial.print(secondsToTimeString(delaySeconds));
    Serial.println();
  } else {
    _oled->print("* ");
    _oled->print(secondsToTimeString(delaySeconds - elapsedSeconds));

    Serial.print("Running(");
    Serial.print(delaySeconds - elapsedSeconds);
    Serial.println(")");
  }
  _oled->print("                ");
  //_oled->display();
}