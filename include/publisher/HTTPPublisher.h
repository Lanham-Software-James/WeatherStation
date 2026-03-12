#pragma once

#include "publisher/Publisher.h"

class HttpPublisher : public Publisher
{
public:
    explicit HttpPublisher(const char* endpoint_url);

    [[nodiscard]] const char* getName() const override;

protected:
    bool onInitialize() override;
    bool onPublish(const Observation& observation) override;

private:
    const char* endpoint_url_;
};
