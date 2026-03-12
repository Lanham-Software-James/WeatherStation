#pragma once

#include <functional>
#include <vector>

#include "config/StationConfig.h"
#include "sensors/Sensor.h"

class SensorFactory
{
public:
    using SensorCreator = std::function<Sensor*(const SensorConfig&)>;

    SensorFactory(
        SensorCreator sht41_creator,
        SensorCreator bmp280_creator);

    std::vector<Sensor*> createSensors(const StationConfig& config);

private:
    SensorCreator sht41_creator_;
    SensorCreator bmp280_creator_;
};
