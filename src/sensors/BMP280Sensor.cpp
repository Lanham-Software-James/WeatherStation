#include "sensors/BMP280Sensor.h"

#include <Arduino.h>
#include <Wire.h>

BMP280Sensor::BMP280Sensor(const SensorConfig& config)
    : config_(config)
{
}

const char* BMP280Sensor::getName() const
{
    return "BMP280";
}

const char* BMP280Sensor::getId() const
{
    return config_.id;
}

bool BMP280Sensor::onInitialize()
{
    if (config_.i2c_address != BMP280_ADDRESS && config_.i2c_address != BMP280_ADDRESS_ALT)
    {
        return false;
    }

    Wire.begin(config_.sda_pin, config_.scl_pin);
    bmp280_.begin(config_.i2c_address);

    return true;
}

bool BMP280Sensor::onRead(Observation& observation)
{
    // Adafruit BMP280 library returns pressure in Pascals.
    const float pressure_pa = bmp280_.readPressure();

    if (pressure_pa <= 0.0f)
    {
        return false;
    }

    observation.pressure_hpa = (pressure_pa / 100.0f) + config_.pressure_offset_hpa;

    return true;
}
