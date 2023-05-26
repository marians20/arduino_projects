#include "durationResolver.h"

#define MIN_ANALOG_VALUE 0
#define MAX_ANALOG_VALUE 1023

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

DurationResolver::DurationResolver(unsigned int potentiometerPin, unsigned int minDuration, unsigned int maxDuration)
  : _potentiometerPin(potentiometerPin),
    _minDuration(minDuration),
    _maxDuration(maxDuration) {
  // linear scaling [f(x) = ax + b] parameters used for transforming analog value into duration
  _a = 1. * (_maxDuration - _minDuration) / (MAX_ANALOG_VALUE - MIN_ANALOG_VALUE);
  _b = 1. * _minDuration - _a * MIN_ANALOG_VALUE;
}

void DurationResolver::begin() {
  pinMode(_potentiometerPin, INPUT_PULLUP);
}

auto DurationResolver::getDuration() -> unsigned int {
  return _a * analogRead(_potentiometerPin) + _b;
}