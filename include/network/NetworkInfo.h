#pragma once

#include <cstdint>

class NetworkInfo
{
    public:
        virtual ~NetworkInfo() = default;
        virtual std::int32_t getRssi() const = 0;
};
