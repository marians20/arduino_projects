#ifndef __DURATION_RESOLVER__
#define __DURATION_RESOLVER__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class DurationResolver {
  public:
    DurationResolver(unsigned int potentiometerPin, unsigned int minDuration, unsigned int maxDuration);
    void begin();
    auto getDuration() -> unsigned int;
  private:
    unsigned int _potentiometerPin;
    unsigned int _minDuration;
    unsigned int _maxDuration;
    double _a;
    double _b;
};
#endif