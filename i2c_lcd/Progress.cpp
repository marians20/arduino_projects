#include "Progress.h"

Progress::Progress(Adafruit_SSD1306 oled): _current(0) {
  _oled = oled;
}

void Progress::Start() {
  _oled.clearDisplay();
  _oled.setTextSize(4);
  _oled.setTextColor(WHITE);
  _oled.display();
}

void Progress::Next() {
  _current = (_current + 1) % 4;
  _oled.clearDisplay();
  _oled.setCursor(0, 0);
  _oled.print(_chars[_current]);
  _oled.display();
}

void Progress::End() {
  _oled.clearDisplay();
  _oled.setTextSize(2);
  _oled.setTextColor(WHITE);
  _oled.display();
}

const char Progress::_chars[] = {'-', '\\', '|', '/'};