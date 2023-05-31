#include "encoderDurationResolver.h"
#include "iDurationResolver.h"

#define STEP 75

EncoderDurationResolver::EncoderDurationResolver(
  uint8_t sw,
  uint8_t dt,
  uint8_t ck,
  unsigned int minDuration,
  unsigned int maxDuration,
  unsigned int defaultDuration)
  : _sw(sw), _dt(dt), _ck(ck), _minDuration(minDuration), _maxDuration(maxDuration), _duration(defaultDuration), _oldPosition(defaultDuration) {
  _encoder = new Encoder(_dt, _ck);
}

EncoderDurationResolver::~EncoderDurationResolver() {
  delete _encoder;
}

auto EncoderDurationResolver::begin() -> void {
  _oldPosition = 0;
}

auto EncoderDurationResolver::getDuration() -> unsigned int {
  return _duration;
}

auto EncoderDurationResolver::loop() -> void {
  long newPosition = _encoder->read();
  if (newPosition == _oldPosition) {
    return;
  }

  if (newPosition > _oldPosition) {
    if (_duration + STEP > _maxDuration) {
      _duration = _maxDuration;
    } else {
      if (_duration == _minDuration) {
        _duration += (STEP - _minDuration);
      } else {
        _duration += STEP;
      }
    }
  } else if (newPosition < _oldPosition) {

    if (_duration - STEP < _minDuration) {
      _duration = _minDuration;
    } else {
      _duration -= STEP;
    }
  }

  Serial.print("Position: ");
  Serial.print(newPosition);
  Serial.print(" Duration: ");
  Serial.println(_duration);

  _oldPosition = newPosition;
}