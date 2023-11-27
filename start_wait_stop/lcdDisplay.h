#ifndef __LCD_DISPLAY_H__
#define __LCD_DISPLAY_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <LiquidCrystal.h>
#include "iDisplay.h"

class LcdDisplay: public IDisplay {
  public:
    LcdDisplay(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    ~LcdDisplay();
    void begin();
    void showTemperatureAndHumidity(ThermometerData data);
    void dispalyTimerStatus(bool isRelayOn, int delaySeconds, int elapsedSeconds);
  private:
    LiquidCrystal* _lcd;
};

#endif