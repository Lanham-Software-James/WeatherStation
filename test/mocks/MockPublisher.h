#pragma once

#include "model/ObservationBatch.h"
#include "publisher/Publisher.h"

class MockPublisher : public Publisher
{
    public:
        bool initialize_result{true};
        bool publish_result{true};

        bool on_initialize_called{false};
        bool on_publish_called{false};

        ObservationBatch last_batch{};

        [[nodiscard]] const char* getName() const override
        {
            return "MockPublisher";
        }

    protected:
        bool onInitialize() override
        {
            on_initialize_called = true;
            return initialize_result;
        }

        bool onPublish(const ObservationBatch& batch) override
        {
            on_publish_called = true;

            if (!publish_result)
            {
                return false;
            }

            last_batch = batch;
            return true;
        }
};
