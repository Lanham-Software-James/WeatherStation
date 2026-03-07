#pragma once

#include <string>

#include "publisher/Publisher.h"
#include "model/Observation.h"

class FakePublisher : public Publisher
{
    public:
        bool initialize_result{true};
        bool publish_result{true};

        bool on_initialize_called{false};
        bool on_publish_called{false};

        Observation last_observation{};

        [[nodiscard]] std::string getName() const override
        {
            return "FakePublisher";
        }

    protected:
        bool onInitialize() override
        {
            on_initialize_called = true;
            return initialize_result;
        }

        bool onPublish(const Observation& observation) override
        {
            on_publish_called = true;

            if (!publish_result)
            {
                return false;
            }

            last_observation = observation;
            return true;
        }
};
