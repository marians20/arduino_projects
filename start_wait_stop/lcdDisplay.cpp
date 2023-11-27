#include "lcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
  _lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
}

LcdDisplay::~LcdDisplay() {
  delete _lcd;
}

void LcdDisplay::begin() {
  _lcd->begin(16, 4);
}

void LcdDisplay::showTemperatureAndHumidity(ThermometerData data) {
  _lcd->setCursor(0, 1);
  _lcd->print(data.temperature);
  _lcd->print((char)223);
  _lcd->print("C ");
  _lcd->print(data.humidity);
  _lcd->print("%");
  _lcd->print("     ");
}

void LcdDisplay::dispalyTimerStatus(bool isRelayOn, int delaySeconds, int elapsedSeconds) {
  _lcd->setCursor(0, 0);
  if (!isRelayOn) {
    _lcd->print("Ready(");
    _lcd->print(delaySeconds);
    _lcd->print(")");
  } else {
    _lcd->print("Running(");
    _lcd->print(delaySeconds - elapsedSeconds);
    _lcd->print(")");
  }
  _lcd->print("                ");
}