#pragma once

class BatteryMonitor
{
    public:
        virtual ~BatteryMonitor() = default;
        virtual float getVoltage() const = 0;
        virtual int getPercentEstimate(float voltage) const = 0;
};
