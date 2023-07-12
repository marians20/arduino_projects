#ifndef __THERMOMETER_H__
#define __THERMOMETER_H__

#include <DHT.h>

class ThermometerData {
  public:
    ThermometerData(float temp, float hum);
    float getTemperature();
    float getHumidity();
    float getHeatIndex();
    void setHeatIndex(float heatIndex);
  private:
    float _temp;
    float _hum;
    float _heatIndex;
};

class Thermometer {
  public:
    Thermometer(uint8_t pin, uint8_t type);
    ~Thermometer();
    void begin();
    ThermometerData read();
  private:
    DHT* _dht;
};

#endif