#pragma once

#include <Wire.h>
#include <SensirionI2cSht4x.h>

#include "sensors/Sensor.h"

class SHT41Sensor : public Sensor
{
    public:
        [[nodiscard]] std::string getName() const override;

    protected:
        bool onInitialize() override;
        bool onRead(Observation& observation) override;

    private:
        void logData(const Observation& observation);

        SensirionI2cSht4x sht4x_;
};
