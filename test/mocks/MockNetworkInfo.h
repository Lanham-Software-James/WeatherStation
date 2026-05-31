#pragma once

#include "network/NetworkInfo.h"

class MockNetworkInfo : public NetworkInfo
{
    public:
        std::int32_t rssi_to_return{-70};

        std::int32_t getRssi() const override
        {
            return rssi_to_return;
        }
};
