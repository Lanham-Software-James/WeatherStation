#pragma once

#include "battery/BatteryMonitor.h"

class MockBatteryMonitor : public BatteryMonitor
{
    public:
        float voltage_to_return{4.20f};
        int percent_to_return{100};

        float getVoltage() const override
        {
            return voltage_to_return;
        }

        int getPercentEstimate(float /*voltage*/) const override
        {
            return percent_to_return;
        }
};
