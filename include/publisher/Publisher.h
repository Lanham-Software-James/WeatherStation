#pragma once

#include "model/ObservationBatch.h"

class Publisher
{
    public:
        virtual ~Publisher() = default;

        bool initialize();
        bool publish(const ObservationBatch& batch);
        [[nodiscard]] bool isInitialized() const;

        [[nodiscard]] virtual const char* getName() const = 0;

    protected:
        virtual bool onInitialize() = 0;
        virtual bool onPublish(const ObservationBatch& batch) = 0;

    private:
        bool initialized_{false};
};
