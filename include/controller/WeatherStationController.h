#pragma once

#include "model/Observation.h"
#include "sensors/Sensor.h"

class WeatherStationController
{
    public:
        explicit WeatherStationController(Sensor* sensor);

        bool initialize();
        bool tick();

    private:
        Sensor* sensor_;
};
