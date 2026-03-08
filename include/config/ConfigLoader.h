#pragma once

#include "config/StationConfig.h"

class ConfigLoader
{
public:
    [[nodiscard]] StationConfig load() const;
};
