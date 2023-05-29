#ifndef DEBOUNCE_MUNT_H
#define DEBOUNCE_MUNT_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class DebounceButton {
public:
  DebounceButton(int b_pin, unsigned long debounceTime,unsigned char inputMode);
  void setupButton();
  auto read() -> int;
private:
  int _b_pin;
  int _buttonPressedLevel;
  unsigned long _debounceTime;
  unsigned char _inputMode;
  unsigned long _debounce;
  uint8_t _status;
};
#endif