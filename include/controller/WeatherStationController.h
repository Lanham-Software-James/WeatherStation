#pragma once

#include <cstdint>
#include <vector>

#include "model/Observation.h"
#include "publisher/Publisher.h"
#include "sensors/Sensor.h"

class WeatherStationController
{
public:
    WeatherStationController(
        const char* station_id,
        const std::vector<Sensor*>& sensors,
        const std::vector<Publisher*>& publishers);

    bool initialize();
    bool tick();

private:
    const char* station_id_;
    std::vector<Sensor*> sensors_;
    std::vector<Publisher*> publishers_;
    std::uint32_t sequence_number_{0};
};
