#include "sensors/BMP280Sensor.h"

#include <Arduino.h>
#include <Wire.h>

namespace
{
    constexpr int SDA_PIN = 21;
    constexpr int SCL_PIN = 22;
}

std::string BMP280Sensor::getName() const
{
    return "BMP280";
}

bool BMP280Sensor::onInitialize()
{
    Wire.begin(SDA_PIN, SCL_PIN);

    // Try the most common default first, then the alternate.
    if (bmp280_.begin(BMP280_ADDRESS))
    {
        
        return true;
    }

    if (bmp280_.begin(BMP280_ADDRESS_ALT))
    {
        return true;
    }

    return false;
}

bool BMP280Sensor::onRead(Observation& observation)
{
    // Adafruit BMP280 library returns pressure in Pascals.
    const float pressure_pa = bmp280_.readPressure();

    if (pressure_pa <= 0.0f)
    {
        return false;
    }

    observation.pressure_hpa = pressure_pa / 100.0f;

    return true;
}
