#ifndef __I_THERMOMETER_H__
#define __I_THERMOMETER_H__

struct ThermometerData {
  float temperature;
  float humidity;
  ThermometerData(float temp, float hum);
};

struct IThermometer {
  virtual auto begin() -> void = 0;
  virtual auto read() -> ThermometerData = 0;
};

#endif