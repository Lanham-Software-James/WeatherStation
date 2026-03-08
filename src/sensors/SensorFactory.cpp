#include "sensors/SensorFactory.h"

#include "sensors/BMP280Sensor.h"
#include "sensors/SHT41Sensor.h"

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
                sensors.push_back(new SHT41Sensor(sensor_config));
                break;

            case SensorType::BMP280:
                sensors.push_back(new BMP280Sensor(sensor_config));
                break;
        }
    }

    return sensors;
}
