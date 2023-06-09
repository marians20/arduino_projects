#include "relay.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Relay::Relay(int relayPin, int ledPin) : _relayPin(relayPin), _ledPin(ledPin) {
  _relayPin = relayPin;
  _ledPin = ledPin;
}

Relay::Relay(int relayPin) : Relay(relayPin, 0){
  _relayPin = relayPin;
  _ledPin = 0;
}

void Relay::setup() {
  pinMode(_ledPin, OUTPUT);
  pinMode(_relayPin, OUTPUT);
}

void Relay::turnOn() {
  _on = true;
  digitalWrite(_relayPin, LOW);
  if (_ledPin > 0) {
    digitalWrite(_ledPin, HIGH);
  }
}

void Relay::turnOff() {
  _on = false;
  digitalWrite(_relayPin, HIGH);
  if (_ledPin > 0) {
    digitalWrite(_ledPin, LOW);
  }
}

void Relay::toggle() {
  if (isOn()) {
    turnOff();
  } else {
    turnOn();
  }
}

auto Relay::isOn() -> bool {
  return _on;
}