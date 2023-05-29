#ifndef __I_DISPLAY_H__
#define __I_DISPLAY_H__

#include "iThermometer.h"

struct IDisplay {
  virtual void begin() = 0;
  virtual void showTemperatureAndHumidity(ThermometerData data) = 0;
  virtual void dispalyTimerStatus(bool isRelayOn, int delaySeconds, int elapsedSeconds) = 0;
};

#endif
