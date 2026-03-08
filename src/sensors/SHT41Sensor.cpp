#include "sensors/SHT41Sensor.h"

namespace
{
    constexpr int SDA_PIN = 21;
    constexpr int SCL_PIN = 22;
}

std::string SHT41Sensor::getName() const
{
    return "SHT41";
}

bool SHT41Sensor::onInitialize()
{
    Wire.begin(SDA_PIN, SCL_PIN);
    sht4x_.begin(Wire, SHT41_I2C_ADDR_44);

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

    observation.temperature_c = temperature_c;
    observation.humidity_pct = humidity_pct;

    return true;
}
