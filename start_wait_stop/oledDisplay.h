#ifndef __OLED_DISPLAY_H__
#define __OLED_DISPLAY_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Adafruit_SSD1306.h>
#include "iDisplay.h"

class OledDisplay: public IDisplay {
  public:
    OledDisplay();
    ~OledDisplay();
    void begin();
    void showTemperatureAndHumidity(ThermometerData data);
    void dispalyTimerStatus(bool isRelayOn, int delaySeconds, int elapsedSeconds);
      virtual void start();
  virtual void commit();
  private:
    Adafruit_SSD1306* _oled;
};

#endif