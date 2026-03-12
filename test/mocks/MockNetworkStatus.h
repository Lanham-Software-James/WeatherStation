#pragma once

#include "publisher/network/NetworkStatus.h"

class MockNetworkStatus : public NetworkStatus
{
public:
    bool connected{true};

    [[nodiscard]] bool isConnected() const override
    {
        return connected;
    }
};
