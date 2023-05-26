#ifndef __RELAY__H__
#define __RELAY__H__
/*
  Relay is used for controlling a relay. A LED can be used to indicate the relay status.
*/
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Relay {
  public:
    Relay(int relayPin, int ledPin);
    Relay(int relayPin);
    void setup();
    void turnOn();
    void turnOff();
    void toggle();
    auto isOn() -> bool;
  private:
    int _relayPin;
    int _ledPin;
    int _on;
};

#endif