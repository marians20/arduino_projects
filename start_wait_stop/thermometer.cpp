#include "thermometer.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

ThermometerData::ThermometerData(float temp, float hum) : temperature(temp), humidity(hum) {}

Thermometer::Thermometer(uint8_t pin, uint8_t sensorType) : _pin(pin), _sensorType(sensorType) {
  _dht = new DHT(_pin, _sensorType);
}

Thermometer::~Thermometer() {
  delete(_dht);
}

void Thermometer::begin() {
  _dht->begin();
}

/*!
    @brief  Read temperature and humidity from sensor
    @param  start
            None
    @return None (void).
*/
auto Thermometer::read() -> ThermometerData {
    return ThermometerData(_dht->readTemperature(), _dht->readHumidity());
}