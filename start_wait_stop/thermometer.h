#ifndef __THERMOMETER_H__
#define __THERMOMETER_H__

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DHT.h>
#include "iThermometer.h"

class Thermometer : public IThermometer {
public:
  Thermometer(uint8_t pin, uint8_t sensorType);
  ~Thermometer();
  auto begin() -> void;
  /*!
    @brief  Read temperature and humidity from sensor
    @param  start
            None
    @return None (void).
*/
  auto read() -> ThermometerData;

private:
  uint8_t _pin;
  uint8_t _sensorType;
  DHT* _dht;
};
#endif