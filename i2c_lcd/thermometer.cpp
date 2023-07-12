#include "thermometer.h"

ThermometerData::ThermometerData(float temp, float hum)
  : _temp(temp), _hum(hum) {}

float ThermometerData::getTemperature() {
    return _temp;
  }

float ThermometerData::getHumidity() {
  return _hum;
}

float ThermometerData::getHeatIndex() {
  return _heatIndex;
}

void ThermometerData::setHeatIndex(float value) {
  _heatIndex = value;
}

Thermometer::Thermometer(uint8_t pin, uint8_t type) {
  _dht = new DHT(pin, type);
}

Thermometer::~Thermometer() {
  delete _dht;
}

void Thermometer::begin() {
  _dht->begin();
}

ThermometerData Thermometer::read() {
  auto result = ThermometerData(_dht->readTemperature(), _dht->readHumidity());
  result.setHeatIndex(_dht->computeHeatIndex(result.getTemperature(), result.getHumidity(), false));
  return result;
}