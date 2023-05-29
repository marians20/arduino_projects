#ifndef __I_DURATION_RESOLVER__
#define __I_DURATION_RESOLVER__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

struct IDurationResolver {
    virtual void begin() = 0;
    virtual auto getDuration() -> unsigned int = 0;
};

#endif