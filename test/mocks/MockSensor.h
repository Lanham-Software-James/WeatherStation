#pragma once

#include <string>

#include "sensors/Sensor.h"

class FakeSensor : public Sensor
{
    public:
        bool initialize_result{true};
        bool read_result{true};

        bool on_initialize_called{false};
        bool on_read_called{false};

        float temperature_to_write{22.5f};
        float humidity_to_write{50.0f};
        float pressure_to_write{1013.25f};

        [[nodiscard]] char* getName() const override
        {
            return "FakeSensor";
        }

        [[nodiscard]] char* getId() const override
        {
            return "fake_sensor_1";
        }

    protected:
        bool onInitialize() override
        {
            on_initialize_called = true;
            return initialize_result;
        }

        bool onRead(Observation& observation) override
        {
            on_read_called = true;

            if (!read_result)
            {
                return false;
            }

            observation.temperature_c = temperature_to_write;
            observation.humidity_pct = humidity_to_write;
            observation.pressure_hpa = pressure_to_write;
            return true;
        }
};
