#pragma once

#include <Adafruit_BMP280.h>

#include "sensors/Sensor.h"
#include "config/StationConfig.h"

class BMP280Sensor : public Sensor
{
    public:
        explicit BMP280Sensor(const SensorConfig& config);

        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] std::string getId() const override;

    protected:
        bool onInitialize() override;
        bool onRead(Observation& observation) override;

    private:
        Adafruit_BMP280 bmp280_;
        SensorConfig config_;
};
