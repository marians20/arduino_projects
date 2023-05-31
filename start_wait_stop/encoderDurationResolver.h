#ifndef __ENCODER_DURATION_RESOLVER__
#define __ENCODER_DURATION_RESOLVER__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Encoder.h>
#include "iDurationResolver.h"

class EncoderDurationResolver : public IDurationResolver {
  public:
    EncoderDurationResolver(uint8_t sw, uint8_t dt, uint8_t ck, unsigned int minDuration, unsigned int maxDuration, unsigned int defaultDuration);
    ~EncoderDurationResolver();
    void begin();
    auto getDuration() -> unsigned int;
    auto loop() -> void;
  private:
    uint8_t _sw;
    uint8_t _dt;
    uint8_t _ck;
    unsigned int _minDuration;
    unsigned int _maxDuration;
    Encoder* _encoder;
    double _a;
    double _b;
    long _oldPosition;
    long _duration;
};
#endif