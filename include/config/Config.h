#pragma once

#include <string>

struct Config
{
    std::string WIFI_SSID = "CHANGE_ME";
    std::string WIFI_PASSWORD = "CHANGE_ME";
};

extern Config CONFIG;
