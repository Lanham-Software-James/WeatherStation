#pragma once

#include "model/ObservationBatch.h"
#include "publisher/Publisher.h"

class MockConstructedPublisher : public Publisher
{
public:
    explicit MockConstructedPublisher(const char* name, const char* id)
        : name_(name), id_(id)
    {
    }

    [[nodiscard]] const char* getName() const override
    {
        return name_;
    }

    [[nodiscard]] const char* getId() const
    {
        return id_;
    }

protected:
    bool onInitialize() override
    {
        return true;
    }

    bool onPublish(const ObservationBatch&) override
    {
        return true;
    }

private:
    const char* name_;
    const char* id_;
};
