#include "sensors/SensorFactory.h"

SensorFactory::SensorFactory(
    SensorCreator sht41_creator,
    SensorCreator bmp280_creator)
    : sht41_creator_(std::move(sht41_creator)),
      bmp280_creator_(std::move(bmp280_creator))
{
}

std::vector<Sensor*> SensorFactory::createSensors(const StationConfig& config)
{
    std::vector<Sensor*> sensors;

    for (const auto& sensor_config : config.sensors)
    {
        if (!sensor_config.enabled)
        {
            continue;
        }

        switch (sensor_config.type)
        {
            case SensorType::SHT41:
                if (sht41_creator_)
                {
                    sensors.push_back(sht41_creator_(sensor_config));
                }
                break;

            case SensorType::BMP280:
                if (bmp280_creator_)
                {
                    sensors.push_back(bmp280_creator_(sensor_config));
                }
                break;
        }
    }

    return sensors;
}
