#pragma once

#include "battery/BatteryMonitor.h"

#include <Arduino.h>

class ADCBatteryMonitor : public BatteryMonitor
{
    public:
        explicit ADCBatteryMonitor(int adc_pin) : adc_pin_(adc_pin) {}

        float getVoltage() const override
        {
            constexpr int kSamples = 50;
            uint32_t sum = 0;
            for (int i = 0; i < kSamples; ++i)
            {
                sum += analogRead(adc_pin_);
                delay(2);
            }
            const float raw = sum / static_cast<float>(kSamples);
            const float adc_voltage = raw * 3.3f / 4095.0f;
            return adc_voltage * 3.2f;
        }

        int getPercentEstimate(float v) const override
        {
            if (v >= 4.20f) return 100;
            if (v >= 4.00f) return 80 + static_cast<int>((v - 4.00f) / 0.20f * 20.0f);
            if (v >= 3.85f) return 60 + static_cast<int>((v - 3.85f) / 0.15f * 20.0f);
            if (v >= 3.70f) return 40 + static_cast<int>((v - 3.70f) / 0.15f * 20.0f);
            if (v >= 3.55f) return 20 + static_cast<int>((v - 3.55f) / 0.15f * 20.0f);
            if (v >= 3.30f) return  5 + static_cast<int>((v - 3.30f) / 0.25f * 15.0f);
            return 0;
        }

    private:
        int adc_pin_;
};
