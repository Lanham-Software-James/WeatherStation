#include "sensors/SHT41Sensor.h"

SHT41Sensor::SHT41Sensor(const SensorConfig& config)
    : config_(config)
{
}

const char* SHT41Sensor::getName() const
{
    return "SHT41Sensor";
}

const char* SHT41Sensor::getId() const
{
    return config_.id;
}

bool SHT41Sensor::onInitialize()
{
    if (config_.i2c_address != SHT41_I2C_ADDR_44 && config_.i2c_address != SHT41_I2C_ADDR_45)
    {
        return false;
    }

    Wire.begin(config_.sda_pin, config_.scl_pin);
    sht4x_.begin(Wire, config_.i2c_address);

    return true;
}

bool SHT41Sensor::onRead(Observation& observation)
{
    float temperature_c = 0.0f;
    float humidity_pct = 0.0f;

    const int16_t error = sht4x_.measureHighPrecision(temperature_c, humidity_pct);

    if (error != 0)
    {
        return false;
    }

    observation.temperature_c = temperature_c + config_.temperature_offset_c;
    observation.humidity_pct = humidity_pct + config_.humidity_offset_pct;

    return true;
}
