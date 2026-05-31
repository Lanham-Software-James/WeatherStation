#pragma once

#include "network/NetworkInfo.h"

#include <WiFi.h>

class WiFiNetworkInfo : public NetworkInfo
{
    public:
        std::int32_t getRssi() const override
        {
            return static_cast<std::int32_t>(WiFi.RSSI());
        }
};
