#pragma once

struct MQTTConfig
{
    const char* BROKER_HOST = "CHANGE_ME";
    int BROKER_PORT = 1883;
    const char* CLIENT_ID = "weather_station";
    const char* TOPIC = "weather/CHANGE_ME/telemetry";
};
