#include "publisher/adapters/WiFiNetworkStatus.h"

#include <WiFi.h>

bool WiFiNetworkStatus::isConnected() const
{
    return WiFi.status() == WL_CONNECTED;
}
