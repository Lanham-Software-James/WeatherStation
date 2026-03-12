#include "doctest.h"

#include "mocks/MockNetworkStatus.h"

TEST_CASE("MockNetworkStatus defaults to connected")
{
    MockNetworkStatus network_status;

    CHECK(network_status.isConnected() == true);
}

TEST_CASE("MockNetworkStatus can be toggled to disconnected")
{
    MockNetworkStatus network_status;
    network_status.connected = false;

    CHECK(network_status.isConnected() == false);
}
