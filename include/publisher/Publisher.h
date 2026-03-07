#pragma once

#include <string>

#include "model/Observation.h"

class Publisher
{
    public:
        virtual ~Publisher() = default;

        bool initialize();
        bool publish(const Observation& observation);
        [[nodiscard]] bool isInitialized() const;

        [[nodiscard]] virtual std::string getName() const = 0;

    protected:
        virtual bool onInitialize() = 0;
        virtual bool onPublish(const Observation& observation) = 0;

    private:
        bool initialized_{false};
};
