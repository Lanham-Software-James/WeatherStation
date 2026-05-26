#pragma once

#include <string>

struct MQTTConfig
{
    std::string BROKER_HOST{"CHANGE_ME"};
    int BROKER_PORT{1883};
    std::string CLIENT_ID{"weather_station"};
    std::string TOPIC{"weather/CHANGE_ME/telemetry"};
};
