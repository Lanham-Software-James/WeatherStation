#pragma once

#include "config/StationConfig.h"
#include "config/NetworkConfig.h"
#include "config/MQTTConfig.h"

struct AppConfig
{
    StationConfig station;
    NetworkConfig network;
    MQTTConfig mqtt;
};
