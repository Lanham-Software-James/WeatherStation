#pragma once

#include "publisher/network/NetworkStatus.h"

class WiFiNetworkStatus : public NetworkStatus
{
public:
    [[nodiscard]] bool isConnected() const override;
};
