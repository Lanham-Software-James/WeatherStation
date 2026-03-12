#pragma once

#include "sensors/Sensor.h"

class MockConstructedSensor : public Sensor
{
public:
    explicit MockConstructedSensor(const char* name, const char* id)
        : name_(name), id_(id)
    {
    }

    [[nodiscard]] const char* getName() const override
    {
        return name_;
    }

    [[nodiscard]] const char* getId() const override
    {
        return id_;
    }

protected:
    bool onInitialize() override
    {
        return true;
    }

    bool onRead(Observation&) override
    {
        return true;
    }

private:
    const char* name_;
    const char* id_;
};
