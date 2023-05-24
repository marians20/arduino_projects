#ifndef __RELAY__H__
#define __RELAY__H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Relay {
  public:
    Relay(int relayPin, int ledPin);
    void setup();
    void turnOn();
    void turnOff();
    void toggle();
    bool isOn();
  private:
    int _relayPin;
    int _ledPin;
    int _on;
};

#endif