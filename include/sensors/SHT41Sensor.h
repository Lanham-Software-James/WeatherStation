#pragma once

#include <Wire.h>
#include <SensirionI2cSht4x.h>

#include "sensors/Sensor.h"
#include "config/StationConfig.h"

class SHT41Sensor : public Sensor
{
    public:
        explicit SHT41Sensor(const SensorConfig& config);

        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] std::string getId() const override;

    protected:
        bool onInitialize() override;
        bool onRead(Observation& observation) override;

    private:
        SensirionI2cSht4x sht4x_;
        SensorConfig config_;
};
