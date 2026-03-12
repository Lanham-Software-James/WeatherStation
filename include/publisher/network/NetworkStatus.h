#pragma once

class NetworkStatus
{
    public:
        virtual ~NetworkStatus() = default;
        [[nodiscard]] virtual bool isConnected() const = 0;
};
