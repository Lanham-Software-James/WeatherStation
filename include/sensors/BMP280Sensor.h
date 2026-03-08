#pragma once

#include <Adafruit_BMP280.h>

#include "sensors/Sensor.h"

class BMP280Sensor : public Sensor
{
    public:
        [[nodiscard]] std::string getName() const override;

    protected:
        bool onInitialize() override;
        bool onRead(Observation& observation) override;

    private:
        Adafruit_BMP280 bmp280_;
};
