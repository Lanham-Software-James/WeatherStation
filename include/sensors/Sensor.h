#pragma once

#include <string>

#include "model/Observation.h"

class Sensor {
    public:
        virtual ~Sensor() = default;

        bool initialize();
        bool read(Observation& observation);
        [[nodiscard]] bool isInitialized() const noexcept;

        [[nodiscard]] virtual const char* getName() const = 0;
        [[nodiscard]] virtual const char* getId() const = 0;

    protected:
        virtual bool onInitialize() = 0;
        virtual bool onRead(Observation& observation) = 0;

    private:
        bool initialized_{false};
};
